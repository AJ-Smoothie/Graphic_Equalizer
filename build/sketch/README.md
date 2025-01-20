#line 1 "E:\\Dropbox\\Projects\\Arduino Sketchbook\\Graphic_Equalizer\\README.md"
# Graphic_Equalizer

Currently, we are setting using pickColor to set the the color values to an array.
This needs to be changed. We already have a beautiful struct of colors.

There needs to be a different color struct for the not-fading pixels

It would appear that I'm making multiple colorMix structs

POINTERS! So you need to remember that a struct is a variable type just like int or float

Pointers must be of the same datatype they are pointing to. A float requires a float pointer,
a int requires a int pointer. When creating a structure, you are able to create multiple structs
by adding commas (e.g struct Var *x, y;). Use this to create a pointer variable(x). This way it has
the same datatype as the stuct. You can then point to another variable (y) of the same datatype,
JUST LIKE float pointer to float.


Step 1: How the fuck does this thing work?

We create an array of pixel objects, as well as a pointer to this array. We'll have to set the pointer = to the array somewhere (in setuP)
 
