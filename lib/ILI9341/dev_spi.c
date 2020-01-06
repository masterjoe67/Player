/*
 * dev_spi.c
 *
 *  Created on: 14 May 2018
 *      Author: Dimitris Tassopoulos
 */

#pragma GCC push_options
#pragma GCC optimize ("O0")

#include "dev_spi.h"
//SPI_TypeDef *sp;
void* dev_spi_init(struct dev_spi * spi, enum en_spi_port port, enum en_spi_mode mode, uint16_t spi_baudrate_prescaller)
{
	//sp = SPI2;
	//sp->SR
	spi->spi = SPI2;
	spi->port = GPIOB;
	spi->nss = GPIO_Pin_12;
	spi->sck = GPIO_Pin_13;
	spi->miso = GPIO_Pin_14;
	spi->mosi = GPIO_Pin_15;
	spi->dma_rx_ch = DMA1_Stream3;
	spi->dma_rx_flags = DMA_FLAG_TCIF3;
	spi->dma_rx_iqrn = DMA1_Stream3_IRQn;
	spi->dma_tx_ch = DMA1_Stream4;
	spi->dma_tx_flags = DMA_FLAG_TCIF4;
	spi->dma_tx_iqrn = DMA1_Stream4_IRQn;
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI2, ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);
	//GPIO_PinAFConfig(GPIOB, GPIO_PinSource12, GPIO_AF_SPI2);
	GPIO_PinAFConfig(GPIOB, GPIO_PinSource13, GPIO_AF_SPI2);
	GPIO_PinAFConfig(GPIOB, GPIO_PinSource14, GPIO_AF_SPI2);
	GPIO_PinAFConfig(GPIOB, GPIO_PinSource15, GPIO_AF_SPI2);
	

	/* GPIO configuration */
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Pin = spi->sck | spi->mosi;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
    GPIO_Init(spi->port, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = spi->nss;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
    GPIO_Init(spi->port, &GPIO_InitStructure);
	spi->port->ODR |= spi->nss;

	GPIO_InitStructure.GPIO_Pin = spi->miso;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
    GPIO_Init(spi->port, &GPIO_InitStructure);

    /* SPI configuration */
    SPI_StructInit(&spi->spi_struct);
    spi->spi_struct.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
    spi->spi_struct.SPI_Mode = SPI_Mode_Master;
    spi->spi_struct.SPI_DataSize = SPI_DataSize_8b;
    spi->spi_struct.SPI_NSS = SPI_NSS_Soft;
    spi->spi_struct.SPI_BaudRatePrescaler = spi_baudrate_prescaller; //SPI_BaudRatePrescaler_2;
    spi->spi_struct.SPI_FirstBit = SPI_FirstBit_MSB;
    switch (mode) {
    case SPI_MODE_0:
        spi->spi_struct.SPI_CPOL = SPI_CPOL_Low;
        spi->spi_struct.SPI_CPHA = SPI_CPHA_1Edge;
    	break;
    case SPI_MODE_1:
        spi->spi_struct.SPI_CPOL = SPI_CPOL_Low;
        spi->spi_struct.SPI_CPHA = SPI_CPHA_2Edge;
    	break;
    case SPI_MODE_2:
        spi->spi_struct.SPI_CPOL = SPI_CPOL_High;
        spi->spi_struct.SPI_CPHA = SPI_CPHA_1Edge;
    	break;
    case SPI_MODE_3:
        spi->spi_struct.SPI_CPOL = SPI_CPOL_High;
        spi->spi_struct.SPI_CPHA = SPI_CPHA_2Edge;
    	break;
    default:
        spi->spi_struct.SPI_CPOL = SPI_CPOL_Low;
        spi->spi_struct.SPI_CPHA = SPI_CPHA_2Edge;
    };
	SPI_CalculateCRC(spi->spi, DISABLE);
    SPI_Init(spi->spi, &spi->spi_struct);
    SPI_Cmd(spi->spi, ENABLE);

    /* DMA configuration */
	/* Enable DMA clock */
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA1, ENABLE);
	/* Initialize DMA struct */
	DMA_StructInit(&spi->dma_struct);

	/* SPI_MASTER_Rx_DMA_Channel configuration */
	DMA_DeInit(spi->dma_rx_ch);
	/* SPI_MASTER_Tx_DMA_Channel configuration */
	DMA_DeInit(spi->dma_tx_ch);

	/* Init default DMA struct */
	spi->dma_struct.DMA_PeripheralBaseAddr = (uint32_t) &SPI2->DR;// = (uint32_t) &spi->spi->DR;
	spi->dma_struct.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
	spi->dma_struct.DMA_MemoryInc = DMA_MemoryInc_Enable;
	spi->dma_struct.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
	spi->dma_struct.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
	spi->dma_struct.DMA_Mode = DMA_Mode_Normal;
	spi->dma_struct.DMA_Priority = DMA_Priority_High;
	//spi->dma_struct.DMA_M2M = DMA_M2M_Disable;

    NVIC_EnableIRQ(spi->dma_rx_iqrn);
    DMA_ITConfig(spi->dma_rx_ch, DMA_IT_TC, ENABLE);
    NVIC_EnableIRQ(spi->dma_tx_iqrn);
    DMA_ITConfig(spi->dma_tx_ch, DMA_IT_TC, ENABLE);

	/* Enable Tx DMA DMA Tx request */
	SPI_I2S_DMACmd(SPI2, SPI_I2S_DMAReq_Rx | SPI_I2S_DMAReq_Tx, ENABLE);

	return spi;
}


inline void dev_spi_start(struct dev_spi * spi)
{
	spi->port->ODR &= ~spi->nss;
}

inline void dev_spi_stop(struct dev_spi * spi)
{
	spi->port->ODR |= spi->nss;
}

static inline void dev_spi_tx(struct dev_spi * spi)
{
	DMA_Init(spi->dma_tx_ch, &spi->dma_struct);
	/* Enable DMA channels */
	DMA_Cmd(spi->dma_tx_ch, ENABLE);
}

static inline void dev_spi_rx(struct dev_spi * spi)
{
	DMA_Init(spi->dma_rx_ch, &spi->dma_struct);
	/* Enable DMA channels */
	DMA_Cmd(spi->dma_rx_ch, ENABLE);
}

inline void dev_spi_wait(struct dev_spi * spi)
{
	/* Wait DMA to finish */
	while(SPI_I2S_GetFlagStatus(spi->spi, SPI_I2S_FLAG_BSY) == SET);
//    /* Disable DMA */
//	DMA_DeInit(spi->dma_tx_ch);
//	DMA_DeInit(spi->dma_rx_ch);
}

inline void dev_spi_send8(struct dev_spi * spi, uint8_t * data, size_t data_len)
{
	/* Configure Tx DMA */
	spi->dma_struct.DMA_Memory0BaseAddr = (uint32_t)(data);
	spi->dma_struct.DMA_BufferSize = data_len;

    spi->dma_struct.DMA_Mode               = DMA_Mode_Normal;
    spi->dma_struct.DMA_MemoryInc          = DMA_MemoryInc_Enable;
	spi->dma_struct.DMA_DIR                = DMA_DIR_MemoryToPeripheral;
    spi->dma_struct.DMA_MemoryDataSize     = DMA_MemoryDataSize_Byte;
    spi->dma_struct.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
	
	

	dev_spi_tx(spi);
}

inline void dev_spi_sendCircular8(struct dev_spi * spi, uint8_t * data, size_t data_len)
{
	/* Configure Tx DMA */
	spi->dma_struct.DMA_Memory0BaseAddr = (uint32_t)(data);
	spi->dma_struct.DMA_BufferSize = data_len;

    spi->dma_struct.DMA_Mode               = DMA_Mode_Circular;
    spi->dma_struct.DMA_MemoryInc          = DMA_MemoryInc_Enable;
	spi->dma_struct.DMA_DIR                = DMA_DIR_MemoryToPeripheral;
    spi->dma_struct.DMA_MemoryDataSize     = DMA_MemoryDataSize_Byte;
    spi->dma_struct.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;

	dev_spi_tx(spi);
}


inline void dev_spi_recv8(struct dev_spi * spi, uint8_t * data, size_t data_len)
{
	/* Configure Rx DMA */
	spi->dma_struct.DMA_Memory0BaseAddr = (uint32_t)(data);
	spi->dma_struct.DMA_BufferSize = data_len;

    spi->dma_struct.DMA_Mode               = DMA_Mode_Normal;
    spi->dma_struct.DMA_MemoryInc          = DMA_MemoryInc_Enable;
	spi->dma_struct.DMA_DIR                = DMA_DIR_PeripheralToMemory;
    spi->dma_struct.DMA_MemoryDataSize     = DMA_MemoryDataSize_Byte;
    spi->dma_struct.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;

	dev_spi_rx(spi);
}

inline void dev_spi_recvCircular8(struct dev_spi * spi, uint8_t * data, size_t data_len)
{
	/* Configure Tx DMA */
	spi->dma_struct.DMA_Memory0BaseAddr = (uint32_t)(data);
	spi->dma_struct.DMA_BufferSize = data_len;

    spi->dma_struct.DMA_Mode               = DMA_Mode_Circular;
    spi->dma_struct.DMA_MemoryInc          = DMA_MemoryInc_Enable;
	spi->dma_struct.DMA_DIR                = DMA_DIR_PeripheralToMemory;
    spi->dma_struct.DMA_MemoryDataSize     = DMA_MemoryDataSize_Byte;
    spi->dma_struct.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;

	dev_spi_rx(spi);
}

inline void dev_spi_sendCircular16(struct dev_spi * spi, uint16_t *data, size_t data_len)
{
    spi->dma_struct.DMA_Memory0BaseAddr = (u32) data;
    spi->dma_struct.DMA_BufferSize     = data_len;

    spi->dma_struct.DMA_Mode               = DMA_Mode_Circular;
    spi->dma_struct.DMA_MemoryInc          = DMA_MemoryInc_Disable;
	spi->dma_struct.DMA_DIR                = DMA_DIR_MemoryToPeripheral;
    spi->dma_struct.DMA_MemoryDataSize     = DMA_MemoryDataSize_HalfWord;
    spi->dma_struct.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;

    dev_spi_tx(spi);
}

inline void dev_spi_send16(struct dev_spi * spi, uint16_t *data, size_t data_len)
{
    spi->dma_struct.DMA_Memory0BaseAddr = (u32) data;
    spi->dma_struct.DMA_BufferSize     = data_len;

    spi->dma_struct.DMA_Mode               = DMA_Mode_Normal;
    spi->dma_struct.DMA_MemoryInc          = DMA_MemoryInc_Enable;
	spi->dma_struct.DMA_DIR                = DMA_DIR_MemoryToPeripheral;
    spi->dma_struct.DMA_MemoryDataSize     = DMA_MemoryDataSize_HalfWord;
    spi->dma_struct.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;

    dev_spi_tx(spi);
}

inline void dev_spi_recv16(struct dev_spi * spi, uint16_t * data, size_t data_len)
{
	/* Configure Rx DMA */
	spi->dma_struct.DMA_Memory0BaseAddr = (uint32_t)(data);
	spi->dma_struct.DMA_BufferSize = data_len;

    spi->dma_struct.DMA_Mode               = DMA_Mode_Normal;
    spi->dma_struct.DMA_MemoryInc          = DMA_MemoryInc_Enable;
	spi->dma_struct.DMA_DIR                = DMA_DIR_PeripheralToMemory;
    spi->dma_struct.DMA_MemoryDataSize     = DMA_MemoryDataSize_HalfWord;
    spi->dma_struct.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;

	dev_spi_rx(spi);
}

inline void dev_spi_recvCircular16(struct dev_spi * spi, uint8_t * data, size_t data_len)
{
	/* Configure Rx DMA */
	spi->dma_struct.DMA_Memory0BaseAddr = (uint32_t)(data);
	spi->dma_struct.DMA_BufferSize = data_len;

    spi->dma_struct.DMA_Mode               = DMA_Mode_Circular;
    spi->dma_struct.DMA_MemoryInc          = DMA_MemoryInc_Enable;
	spi->dma_struct.DMA_DIR                = DMA_DIR_PeripheralToMemory;
    spi->dma_struct.DMA_MemoryDataSize     = DMA_MemoryDataSize_HalfWord;
    spi->dma_struct.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;

	dev_spi_rx(spi);
}


void dev_spi_set8(struct dev_spi * spi)
{
	SPI2->CR1 &= ~SPI_CR1_SPE;  // DISABLE SPI
    SPI2->CR1 &= ~SPI_CR1_DFF;  // SPI 8
    SPI2->CR1 |= SPI_CR1_SPE;   // ENABLE SPI
}

void dev_spi_set16(struct dev_spi * spi)
{
    SPI2->CR1 &= ~SPI_CR1_SPE; // DISABLE SPI
    SPI2->CR1 |= SPI_CR1_DFF;   // SPI 16
    SPI2->CR1 |= SPI_CR1_SPE;   // ENABLE SPI
}




void DMA1_Stream3_IRQHandler(void) {
	if (DMA_GetITStatus(DMA1_Stream3, DMA_IT_TCIF3) == SET) {
        DMA_Cmd(DMA1_Stream3, DISABLE);
		DMA_ClearITPendingBit(DMA1_Stream3, DMA_IT_TCIF3);
    }
}

void DMA1_Stream4_IRQHandler(void) {
	if (DMA_GetITStatus(DMA1_Stream4, DMA_IT_TCIF4) == SET) {
	    DMA_Cmd(DMA1_Stream4, DISABLE);
		DMA_ClearITPendingBit(DMA1_Stream4, DMA_IT_TCIF4);
    }
}

#pragma GCC pop_options
