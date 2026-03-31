# Flash script for a_nick_blink

$openocd = "C:\openocd\bin\openocd.exe"
$scripts = "C:\openocd\scripts"
$elf = "build/stm32l476/app/a_nick_blink/a_nick_blink.elf"

& $openocd `
  -s $scripts `
  -f "interface/stlink.cfg" `
  -f "target/stm32l4x.cfg" `
  -c "program $elf verify reset exit"