# cli

The cli module is composed of three directories:
1. `cli_manager` - The main cli manager that registers commands and routes inputs and to those commands.
2. `cli_commands` - Contains user-definable commands. A command is composed of an initialization method, a command handler, and a command struct.
3. `cli_core` - Holds the base-class for commands. Commands have a name (alias), description, and a function pointer to the command handler.

Initailize a cli instance and register user commands in `<your_app>/config/cli_config.h`.
Board support should have usart and enable sending and receiving.