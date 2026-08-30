#include "max30102.h"

static int max30102_is_valid_dev(const max30102_dev_t *dev)
{
    return dev != NULL && dev->read != NULL && dev->write != NULL;
}

int max30102_read_reg(max30102_dev_t *dev, uint8_t reg, uint8_t *value)
{
    if (!max30102_is_valid_dev(dev) || value == NULL) {
        return -1;
    }
    return dev->read(dev->ctx, reg, value, 1u);
}

int max30102_write_reg(max30102_dev_t *dev, uint8_t reg, uint8_t value)
{
    if (!max30102_is_valid_dev(dev)) {
        return -1;
    }
    return dev->write(dev->ctx, reg, &value, 1u);
}

int max30102_read_regs(max30102_dev_t *dev, uint8_t start_reg, uint8_t *buffer, size_t len)
{
    if (!max30102_is_valid_dev(dev) || buffer == NULL || len == 0u) {
        return -1;
    }
    return dev->read(dev->ctx, start_reg, buffer, len);
}

int max30102_read_fifo_ir(max30102_dev_t *dev, uint32_t *ir_value)
{
    uint8_t raw[3];

    if (ir_value == NULL) {
        return -1;
    }
    if (max30102_read_regs(dev, MAX30102_REG_FIFO_DATA, raw, sizeof(raw)) != 0) {
        return -1;
    }

    *ir_value = (((uint32_t)raw[0] << 16) | ((uint32_t)raw[1] << 8) | (uint32_t)raw[2]) & 0x3FFFFu;
    return 0;
}

float max30102_estimate_bpm(const uint32_t *ir_samples, size_t sample_count, float sample_rate_hz)
{
    float mean = 0.0f;
    uint32_t min_v;
    uint32_t max_v;
    float threshold;
    size_t peak_indexes[64];
    size_t peak_count = 0u;
    size_t min_distance;
    size_t i;
    float avg_interval = 0.0f;

    if (ir_samples == NULL || sample_count < 3u || sample_rate_hz <= 0.0f) {
        return 0.0f;
    }

    min_v = ir_samples[0];
    max_v = ir_samples[0];
    for (i = 0u; i < sample_count; ++i) {
        mean += (float)ir_samples[i];
        if (ir_samples[i] < min_v) {
            min_v = ir_samples[i];
        }
        if (ir_samples[i] > max_v) {
            max_v = ir_samples[i];
        }
    }
    mean /= (float)sample_count;

    threshold = mean + ((float)(max_v - min_v) * 0.2f);
    min_distance = (size_t)(sample_rate_hz * 0.3f);
    if (min_distance == 0u) {
        min_distance = 1u;
    }

    for (i = 1u; i + 1u < sample_count; ++i) {
        int is_peak = ir_samples[i] > ir_samples[i - 1u] &&
                      ir_samples[i] >= ir_samples[i + 1u] &&
                      (float)ir_samples[i] >= threshold;

        if (!is_peak) {
            continue;
        }

        if (peak_count > 0u && (i - peak_indexes[peak_count - 1u]) < min_distance) {
            if (ir_samples[i] > ir_samples[peak_indexes[peak_count - 1u]]) {
                peak_indexes[peak_count - 1u] = i;
            }
            continue;
        }

        if (peak_count < (sizeof(peak_indexes) / sizeof(peak_indexes[0]))) {
            peak_indexes[peak_count++] = i;
        }
    }

    if (peak_count < 2u) {
        return 0.0f;
    }

    for (i = 1u; i < peak_count; ++i) {
        avg_interval += (float)(peak_indexes[i] - peak_indexes[i - 1u]);
    }
    avg_interval /= (float)(peak_count - 1u);

    if (avg_interval <= 0.0f) {
        return 0.0f;
    }

    {
        float bpm = 60.0f * sample_rate_hz / avg_interval;
        if (bpm < 30.0f || bpm > 220.0f) {
            return 0.0f;
        }
        return bpm;
    }
}
