# Timebase — Quick Guide

Two sides to using the timebase: the **BSP** wires the hardware up once, and your
**app** (in `main` or a wrapper) just calls a few functions.

- Interface: [`common/drivers/time/timebase.h`](../common/drivers/time/timebase.h)
- STM32H7 driver: [`common/drivers/platform/stm32h7/st_timebase.h`](../common/drivers/platform/stm32h7/st_timebase.h)

---

## BSP side (set up once)

The BSP owns everything hardware-specific: the peripheral clock, the NVIC, and
the IRQ vector. The driver does **not** touch these on purpose.

```cpp
// 1. Construct: timer, its IRQ, timer input clock (Hz), IRQ enabled.
StTimebaseParams params{TIM3, TIM3_IRQn, 4'000'000, true};
HwTimebase timebase{params};

bool bsp_init()
{
    // 2. Enable the timer's peripheral clock.
    RCC->APB1LENR |= RCC_APB1LENR_TIM3EN;

    // 3. Configure the NVIC line (driver never touches NVIC).
    NVIC_DisableIRQ(TIM3_IRQn);
    NVIC_SetPriority(TIM3_IRQn, 5U);

    // 4. Initialize the driver.
    timebase.init();

    // 5. Unmask the interrupt.
    NVIC_ClearPendingIRQ(TIM3_IRQn);
    NVIC_EnableIRQ(TIM3_IRQn);
    return true;
}

// 6. Route the timer's IRQ vector into the driver.
extern "C" void TIM3_IRQHandler(void) { timebase.handle_irq(); }
```

After this, the app gets the timebase via `get_board().timebase`.

> Timer input clock must be an exact multiple of 1 MHz (e.g. 4 MHz). Full example:
> [`h723_board.cc`](../app/timebase_test/bsp_h723/h723_board.cc).

---

## App side (what you call in `main` / a wrapper)

These are the only functions you actually need:

| Call                              | Purpose                                           |
| --------------------------------- | ------------------------------------------------- |
| `timebase.start()`                | Begin counting. Call once before reading time.    |
| `timebase.uptime_us()`            | Current time in microseconds (monotonic).         |
| `timebase.elapsed_since_us(t0)`   | Microseconds elapsed since a saved `uptime_us()`. |
| `timebase.set_freq(hz)`           | **Adjust the timebase** counter frequency.        |
| `timebase.stop()`                 | Pause counting.                                   |

```cpp
int main()
{
    bsp_init();
    Board board = get_board();

    board.timebase.start();                         // enable the timebase

    // Measure a duration.
    const uint64_t t0 = board.timebase.uptime_us();
    do_work();
    const uint64_t took_us = board.timebase.elapsed_since_us(t0);

    // Do something every 100 ms without blocking.
    uint64_t last = board.timebase.uptime_us();
    while (1)
    {
        if (board.timebase.elapsed_since_us(last) >= 100'000)
        {
            last = board.timebase.uptime_us();
            // ... periodic task ...
        }
    }
}
```

---

## Adjusting the timebase

`set_freq()` is the function to change the timebase rate. The timer input clock
is fixed by the BSP, so you only pass the new counter frequency — and it must
divide that input clock exactly.

```cpp
board.timebase.set_freq(1'000'000);   // 1 MHz   -> 1 tick = 1 us  (default)
board.timebase.set_freq(500'000);     // 500 kHz -> 1 tick = 2 us
```

Two things to know:

- `uptime_us()` always reports **real microseconds**, unchanged by `set_freq()`.
- `set_freq()` **resets** the counter and overflow count to 0 — re-capture any
  saved `uptime_us()` timestamps afterward.

Need a specific overflow period instead of a frequency? Use
`set_period(std::chrono::microseconds{...})`.
