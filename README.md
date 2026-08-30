# max30102
OpenWRT package for reading MAX30102 heart beat sensor for BeagleBone Black

## C driver API

Minimal C driver was added with:

- register read/write helpers for MAX30102 over I2C
- FIFO IR sample read (`MAX30102_REG_FIFO_DATA`)
- basic heart-rate estimation (`max30102_estimate_bpm`) from IR samples

### Build and run tests

```bash
make test
```
