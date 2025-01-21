# Creating Branch

Why am I creating a branch? Well I'm really having issues with the top pixel. It's very fluttery do the way I'm reading from
the ADC and it does not look good. I'm having issues with the volume being mapped from the ADC into a 0-7 column for the volume.
What I'm doing is creating another brance where instead of doing a volume from 0 - 7, the getVolume() function actually does
a percentage of volume. I will then convert this volume elsewhere to apply to the grid.

Currently the last pixel is not being displayed at the top end.
