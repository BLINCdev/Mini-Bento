# Mini Bento Arm
The Mini Bento Arm is a 3D printable robotic arm designed specifically for education and take home rehabilitation applications. Building off the success of the full-scale [Bento Arm](https://github.com/BLINCdev/Bento-Arm-Hardware) (which has been used extensively as a robotic prosthesis for myoelectric training and research), the 1/3 scale Mini Bento Arm provides many of the same features for a fraction of the cost (and space!). The Dynamixel XL330-M288-T actuators are used in the arm to provide up to 4 movements and include joint feedback that can be used for sensory feedback or machine learning. The arm can be controlled through a computer via our brachI/Oplexus software or directly via the onboard OpenRB-150 controller through an Arduino interface. 

## Build your own Mini Bento Arm
The Mini Bento Arm consists of off-the-shelf X-series Dynamixel actuators and brackets, metric fasteners, and custom 3D printed parts. Instructions for building the arm and setting up the actuators can be found in _'Mini Bento Arm - Assembly Guide.doc'_. The assembly guide will be your main goto document while printing and building the arm and includes references to the other files in the repository:

* __Bill of Materials:__ A detailed list of the off-the-shelf materials and parts required to build the Mini Bento Arm. Note: We have listed the distributors that we have used in the past as a point of reference, but be aware that there are multiple distributor options for each part and that we do not specifically endorse the listed distributors.
* __3D Printing Guide:__ The printing guide includes a list of the 3D prints and print settings (Table 1) and how they relate to the part names in the main Solidworks Assembly.
* __3D Print Files:__ The folder that contains the raw STL files as well as the .THING and .x3g files for 3D printing on a Makerbot Replicator 2. NOTE: The STL files have been adjusted to print with proper tolerances in the PLA material.
* __Solidworks Files:__ The folder that contains the Solidworks part and assembly files. The main assembly file for the entire arm is called 'mini_bento_assembly_rev2.sldasm'. The parts and assemblies were modelled in Solidworks 2016 SP 1. The tolerances on specific parts can be dialed into other printers by changing the PLA_shrink value in 'common_dimensions.txt' to the amount that small holes typically shrink in your printer and then rebuilding/exporting the parts as .
* __STEP Files:__ This folder contains the STEP file for the main assembly and should be importable into most alternate 3D CAD softwares.


## Contributing
You can share your experience, new design, ideas, feature requests, or questions by[contacting us](https://blincdev.ca/contact/).

If you would like to contribute to future official releases of the Mini Bento Arm we recommend contacting us through our wbsite to coordinate with our development team. To get started you will need to [fork this repo](https://help.github.com/articles/using-pull-requests/) and once your modification or enhancement is complete submit a [pull request](https://help.github.com/articles/using-pull-requests/).

## License
The Mini Bento Arm hardware is released under the [Creative Commons BY-SA 4.0](http://creativecommons.org/licenses/by-sa/4.0/) license. A local copy of the license that was forked from [https://github.com/idleberg/Creative-Commons-4.0-Markdown](https://github.com/idleberg/Creative-Commons-4.0-Markdown) is available in the repository. 

