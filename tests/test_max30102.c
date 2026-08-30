#include <assert.h>
#include <math.h>
#include <stdint.h>
#include <string.h>

#include "max30102.h"

typedef struct {
    uint8_t regs[256];
} fake_i2c_t;

static int fake_read(void *ctx, uint8_t reg, uint8_t *data, size_t len)
{
    fake_i2c_t *bus = (fake_i2c_t *)ctx;
    memcpy(data, &bus->regs[reg], len);
    return 0;
}

static int fake_write(void *ctx, uint8_t reg, const uint8_t *data, size_t len)
{
    fake_i2c_t *bus = (fake_i2c_t *)ctx;
    memcpy(&bus->regs[reg], data, len);
    return 0;
}

static void test_register_read_write(void)
{
    fake_i2c_t bus = {0};
    max30102_dev_t dev = {.ctx = &bus, .read = fake_read, .write = fake_write};
    uint8_t value = 0u;

    assert(max30102_write_reg(&dev, 0x09u, 0xABu) == 0);
    assert(max30102_read_reg(&dev, 0x09u, &value) == 0);
    assert(value == 0xABu);
}

static void test_fifo_ir_parse(void)
{
    fake_i2c_t bus = {0};
    max30102_dev_t dev = {.ctx = &bus, .read = fake_read, .write = fake_write};
    uint32_t ir = 0u;

    bus.regs[MAX30102_REG_FIFO_DATA] = 0x3Fu;
    bus.regs[MAX30102_REG_FIFO_DATA + 1u] = 0xFFu;
    bus.regs[MAX30102_REG_FIFO_DATA + 2u] = 0xEEu;

    assert(max30102_read_fifo_ir(&dev, &ir) == 0);
    assert(ir == 0x3FFEEu);
}

static void test_bpm_estimation(void)
{
    uint32_t samples[] = {
        1000, 1000, 1000, 1050, 1300, 1800, 1300, 1050, 1000, 1000, 1000, 1000, 1000, 1000, 1000,
        1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1050, 1300, 1800, 1300, 1050, 1000, 1000,
        1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1050, 1300,
        1800, 1300, 1050, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000,
        1000, 1000, 1050, 1300, 1800, 1300, 1050, 1000};
    float bpm = max30102_estimate_bpm(samples, sizeof(samples) / sizeof(samples[0]), 25.0f);

    assert(fabsf(bpm - 75.0f) < 2.0f);
}

int main(void)
{
    test_register_read_write();
    test_fifo_ir_parse();
    test_bpm_estimation();
    return 0;
}
