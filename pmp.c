///////////////////////////////////////////////////////////////////////
// pmp: Pico Module Player
//
// Derived from Mike Field's PiPico_Drummer project:
//
// ---
// drummer.c : a simple Drum machine for the Raspberry Pi Pico
//
// (c) 2021 Mike Field <hamster@snap.net.nz>
//
// Really just a test of making an I2S interface. Plays a few different
// bars of drum patterns
// ---
//
// Changes (c) 2021 Matt Evans

///////////////////////////////////////////////////////////////////////
// This section is all the hardware specific DMA stuff
///////////////////////////////////////////////////////////////////////
#include <stdio.h>
#include <memory.h>
#include "hardware/dma.h"
#include "hardware/irq.h"
#include "pio_i2s.pio.h"
#include "lmp/littlemodplayer.h"

#define PIO_I2S_CLKDIV 44.25F
#define N_BUFFERS 4
#define BUFFER_SIZE 49

static int dma_chan;
static uint32_t buffer[N_BUFFERS][BUFFER_SIZE];
static volatile int buffer_playing = 0;
static int buffer_to_fill = 0;

static void dma_handler() {
    // Clear the interrupt request.
    dma_hw->ints0 = 1u << dma_chan;
    // Give the channel a new wave table entry to read from, and re-trigger it
    dma_channel_set_read_addr(dma_chan, buffer[buffer_playing], true);
    if(buffer_playing == N_BUFFERS-1) 
       buffer_playing = 0;
    else
       buffer_playing++;
}


int setup_dma(void) {
     
    //////////////////////////////////////////////////////
    // Set up a PIO state machine to serialise our bits
    uint offset = pio_add_program(pio0, &pio_i2s_program);
    pio_i2s_program_init(pio0, 0, offset, 9, PIO_I2S_CLKDIV);

    //////////////////////////////////////////////////////
    // Configure a channel to write the buffers to PIO0 SM0's TX FIFO, 
    // paced by the data request signal from that peripheral.
    dma_chan = dma_claim_unused_channel(true);
    dma_channel_config c = dma_channel_get_default_config(dma_chan);
    channel_config_set_transfer_data_size(&c, DMA_SIZE_32);
    channel_config_set_read_increment(&c, 1); 
    channel_config_set_dreq(&c, DREQ_PIO0_TX0);

    dma_channel_configure(
        dma_chan,
        &c,
        &pio0_hw->txf[0], // Write address (only need to set this once)
        NULL,
        BUFFER_SIZE,        
        false             // Don't start yet
    );

    // Tell the DMA to raise IRQ line 0 when the channel finishes a block
    dma_channel_set_irq0_enabled(dma_chan, true);

    // Configure the processor to run dma_handler() when DMA IRQ 0 is asserted
    irq_set_exclusive_handler(DMA_IRQ_0, dma_handler);
    irq_set_enabled(DMA_IRQ_0, true);

}


///////////////////////////////////////////////////////////////////////////////////
// Everything from here down is non-h/w specific
///////////////////////////////////////////////////////////////////////////////////


#define SAMPLE_RATE    ((int)(125000000/PIO_I2S_CLKDIV/32/2))


static const uint8_t modfile[] = {
#include "modfile.h"
};

mps_t modstate;

static void fill_buffer(void) {
    if(buffer_playing == buffer_to_fill)
       return;

    lmp_fill_buffer(&modstate, (uint16_t *)buffer[buffer_to_fill], BUFFER_SIZE*2, LMP_STEREO_SOFT);

    buffer_to_fill = (buffer_to_fill+1)%N_BUFFERS;
}


int main(void)
{
	lmp_init(&modstate, modfile);

	buffer_playing = -1; // To stop the filling routine from stalling
	for(int i = 0; i < N_BUFFERS; i++) {
		fill_buffer();
	}
	buffer_playing = N_BUFFERS-1;

	////////////////////////////////////////////////////////////
	// Set up the DMA transfers, then call the 
	// handler to trigger the first transfer
	////////////////////////////////////////////////////////////
	setup_dma();
	dma_handler();

	////////////////////////////////////////////////////////////
	// Fill buffers with new samples as they are consumed
	// by the DMA transfers
	////////////////////////////////////////////////////////////
	while (true) {
		fill_buffer();
	}
}
