#ifndef MAX30102_H
#define MAX30102_H

#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#define MAX30102_I2C_ADDR          0x57u
#define MAX30102_REG_FIFO_DATA     0x07u
#define MAX30102_REG_FIFO_WR_PTR   0x04u
#define MAX30102_REG_FIFO_RD_PTR   0x06u

typedef int (*max30102_i2c_read_fn)(void *ctx, uint8_t reg, uint8_t *data, size_t len);
typedef int (*max30102_i2c_write_fn)(void *ctx, uint8_t reg, const uint8_t *data, size_t len);

typedef struct {
    void *ctx;
    max30102_i2c_read_fn read;
    max30102_i2c_write_fn write;
} max30102_dev_t;

int max30102_read_reg(max30102_dev_t *dev, uint8_t reg, uint8_t *value);
int max30102_write_reg(max30102_dev_t *dev, uint8_t reg, uint8_t value);
int max30102_read_regs(max30102_dev_t *dev, uint8_t start_reg, uint8_t *buffer, size_t len);
int max30102_read_fifo_ir(max30102_dev_t *dev, uint32_t *ir_value);

float max30102_estimate_bpm(const uint32_t *ir_samples, size_t sample_count, float sample_rate_hz);

#ifdef __cplusplus
}
#endif

#endif
