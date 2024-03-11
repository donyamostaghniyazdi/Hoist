Interactive Hoist Simulator

This project is an interactive simulator for a hoist system with 2 degrees of freedom (d.o.f.), designed as the first assignment for ARP (Advanced Robotics Programming) students. The simulator allows for movement along the X and Z axes using two separate motors, `motX` and `motZ`, with movement range fixed from 0 to a maximum distance of 1.

#Features

Two Different Consoles:** Users can activate the hoist using two different consoles – `CmdConsole` for managing movement and commands, and `InspectionConsole` for viewing the current position on the axes and for stopping or resetting the hoist position.
 **Five Processes:** The system comprises five processes – `CmdConsole`, `InspectionConsole`, `MotorX`, `MotorZ`, and `Watchdog` – to manage the hoist's operation.
 **Pipes and Signals:** Communication between processes is managed using pipes for one-to-one connections. Signals are implemented for interruptions, such as stopping the hoist from the `InspectionConsole` and handling emergency stops.

 Installation

Clone this repository to your local machine. Make sure you have GCC installed to compile the source code.


 Usage

After compilation, start the main process to initiate the simulator.


The main process will launch the required subprocesses (`CmdConsole`, `InspectionConsole`, `MotorX`, `MotorZ`, and `Watchdog`). Use the command and inspection consoles to interact with the hoist system.

 Command Console

Use the following commands in the `CmdConsole` to control the hoist:

- `T`: Move up.
- `G`: Move down.
- `F`: Move left.
- `H`: Move right.
- `Z`: Stop Z movement.
- `X`: Stop X movement.

   Inspection Console

The `InspectionConsole` displays the current position of the hoist on the X and Z axes. It also allows the user to reset the hoist position or trigger an emergency stop.

 Contributing

Contributions to improve the simulator are welcome. Please follow the standard fork and pull request workflow.

