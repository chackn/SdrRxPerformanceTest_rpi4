/* metatest.c */

#include <stdio.h>
#include <libbladeRF.h>
#include <stdlib.h>

#define PARAM_FREQ 2400000000
#define PARAM_SAMPRATE 200000000
#define PARAM_ITER 100000
#define PARAM_NUMSAMPLES 8192

const unsigned int num_buffers = 16;
const unsigned int buffer_size = 1024*16; /* Must be a multiple of 1024 */
const unsigned int num_transfers = 8;
const unsigned int timeout_ms = 3500;

int main(int argc, const char *argv[])
{
        struct bladerf *dev;
        void *samples = malloc(4*PARAM_NUMSAMPLES/1000);
        struct bladerf_metadata meta;
        int stat;
	printf("Sample Rate=%d(ksps)\n", PARAM_SAMPRATE);
        bladerf_open(&dev, "");
        /* Configure both the device's x1 RX and TX channels for use with the
        * synchronous
        * interface. SC16 Q11 samples *without* metadata are used. */
        stat = bladerf_sync_config(dev, BLADERF_RX_X1,
                BLADERF_FORMAT_SC16_Q11_META, num_buffers,
                buffer_size, num_transfers, timeout_ms);
        bladerf_set_frequency(dev, BLADERF_CHANNEL_RX(0), PARAM_FREQ);
        bladerf_set_sample_rate(dev, BLADERF_CHANNEL_RX(0), PARAM_SAMPRATE, NULL);
        bladerf_enable_module(dev, BLADERF_CHANNEL_RX(0), true);
        for (int i; i < PARAM_ITER; i++)
        {
                bladerf_sync_rx(dev, samples, PARAM_NUMSAMPLES, &meta, timeout_ms);
                if (meta.status & BLADERF_META_STATUS_OVERRUN)
                        printf("Buffer Overrun! i=%d\n", i);
                else if (meta.status & BLADERF_META_STATUS_UNDERRUN)
                        printf("Buffer Underrun! i=%d\n", i);
        }
        bladerf_close(dev);
}

