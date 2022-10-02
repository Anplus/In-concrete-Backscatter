# Reader

We prototype the reader by using a multifunctional data acquisition board (DAQ) USB-3213 from Smacq. The DAQ transmits the 200 kHz signal to power up and send command to the tag. It also collects and decodes the uplink backscatter signal. The code is tested on MATLAB 2021a.

# How to run?

- Install the driver by installing the gusb.inf.
- Run the decoder.m.