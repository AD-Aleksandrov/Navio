#include "SPIdev.h"


SPIdev::~SPIdev()
{
    shutdown();
}

int SPIdev::initialize(void)
{
    if (spi_fd >= 0) {
        return 0;
    }

    spi_fd = ::open(spidev, O_RDWR);

    if (spi_fd < 0) {
        printf("Error: SPI initialize failed\n");
        return spi_fd;
    }

    return 0;
}

int SPIdev::shutdown(void)
{
    if (spi_fd < 0) {
        return 0;
    }

    int ret = ::close(spi_fd);

    if (ret < 0) {
        printf("Error: SPI shutdown failed\n");
        return ret;
    }

    spi_fd = -1;
    return 0;
}

int SPIdev::writeReg(uint8_t address, uint8_t data)
{
    uint8_t tx[2] = {address, data};
    uint8_t rx[2] = {0};

    int ret = transfer(tx, rx, 2);

    if (ret != 3) {
        printf("Error: SPI writeReg failed\n");
        return -1;
    }

    return 0;
}

int SPIdev::readReg(uint8_t address, uint8_t &value)
{
    uint8_t tx[2] = {address | READ_FLAG, 0};
    uint8_t rx[2] = {0};

    int ret = transfer(tx, rx, 2);

    if (ret != 2) {
        printf("Error: SPI readReg failed\n");
        return -1;
    }

    value = rx[1];
    return 0;
}

int SPIdev::readRegs(uint8_t address, uint8_t *buf, unsigned int length)
{
    uint8_t tx[255] = {0};
    uint8_t rx[255] = {0};

    tx[0] = address | READ_FLAG;

    int ret = transfer(tx, rx, length + 1);

    if (ret != length + 1) {
        printf("Error: SPI readRegs failed\n");
        return -1;
    }

    for(int i = 0; i < length; i++)
        buf[i] = rx[i + 1];

    return 0;
}

int SPIdev::transfer(uint8_t *tx,
                     uint8_t *rx,
                     unsigned int length,
                     unsigned int speed_hz,
                     unsigned char bits_per_word,
                     unsigned short delay_usecs)
{
    spi_ioc_transfer spi_transfer;
    memset(&spi_transfer, 0, sizeof(spi_ioc_transfer));

    spi_transfer.tx_buf = (unsigned long)tx;
    spi_transfer.rx_buf = (unsigned long)rx;
    spi_transfer.len = length;
    spi_transfer.speed_hz = speed_hz;
    spi_transfer.bits_per_word = bits_per_word;
    spi_transfer.delay_usecs = delay_usecs;

    return ioctl(spi_fd, SPI_IOC_MESSAGE(1), &spi_transfer);
}
