# RollerCoasterCreator
Basic roller coaster creator program

This is a project I created for my intro to C programming class using CodeBlocks.  This program creates a very simple coaster builder, where the user can choose what 
elements to add in what order and any amount.

The coaster always starts with a drop from some user set height.  The coaster traverses a circular curve downwards until the user set angle of descent is reached.  Then, at 
about 1/3 of the way from 0 height, the coaster traverses a circular curve until the coaster is parallel with the ground.  After this drop sequence is complete, the user then
can select 34 different elements: straight section, brake section, vertical loop, and end element selection.  With both the brake and straight sections, the user specifies the length of these sections.
For the vertical loop, the user specifies the height of the loop.  For simplicity, the loop is a perfect circle, which leads to many unsafe levels of G's.

This program runs in real-time, displaying current statisitics about every 0.5 seconds.  The statistics displayed include the X postion, Y position, velocity, angle (if at an
angle that is not 0), and G's.  In addition, a message will display next to the amount of G's telling the relative danger of the current amount of G's.

The simulation will end in three different ways.  If the coaster successfully reaches the end of user selected elements with some velocity still left, the simulation ends.
If the coaster runs out of speed on a brake or straight section, the simulation ends.  If the coaster runs out of speed while traversing the vertical loop, the simulation
will end.  For each of these three scenarios, a corresponding simulation end message will be displayed.
