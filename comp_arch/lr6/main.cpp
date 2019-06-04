#include <iostream>
#include <libusb-1.0/libusb.h>
#include <stdio.h>
using namespace std;

void printdev(libusb_device *dev);

void printSerialNumber(libusb_device *dev);
int main(){
    libusb_device **devs; // указатель на указатель на устройство,
    // используется для получения списка устройств
    libusb_context *ctx = NULL; // контекст сессии libusb
    int r; // для возвращаемых значений
    ssize_t cnt; // число найденных USB-устройств
    ssize_t i; // индексная переменная цикла перебора всех устройств
    // инициализировать библиотеку libusb, открыть сессию работы с libusb
    r = libusb_init(&ctx); //инициализация работы с библиотекой

    if(r < 0){
        fprintf(stderr,
                "Ошибка: инициализация не выполнена, код: %d.\n", r);
        return 1;
    }

    // задать уровень подробности отладочных сообщений
    libusb_set_debug(ctx, 3);

    // получить список всех найденных USB- устройств
    cnt = libusb_get_device_list(ctx, &devs);

    if(cnt < 0){
        fprintf(stderr,
                "Ошибка: список USB устройств не получен.\n %d", r);
        return 1;
    }

    printf("найдено устройств: %zd\n", cnt);
    printf("=============================="
           "=============================\n");
    printf("  |* класс устройства\n");
    printf("  |  |* подкласс устройства\n");
    printf("  |  |  |* протокол устройства\n");
    printf("  |  |  |  |* идентификатор производителя\n");
    printf("  |  |  |  |    |* идентификатор устройства\n");
    printf("  |  |  |  |    |     |*серийный номер\n");

    for(i = 0; i < cnt; i++) { // цикл перебора всех устройств
    printf("%zd ",i);
        printdev(devs[i]); // печать параметров устройства
    }
    printf("=============================="
           "=============================\n");

    // освободить память, выделенную функцией получения списка устройств
    libusb_free_device_list(devs, 1);

    libusb_exit(ctx); // завершить работу с библиотекой libusb,
    // закрыть сессию работы с libusb
    return 0;
}
void printSerialNumber(libusb_device *dev)
{
    unsigned char uSerialNumber[255] = {};
    libusb_device_descriptor desc; // дескриптор устройства
    libusb_device_handle *handle; //a device handle

    int r = libusb_get_device_descriptor(dev, &desc);
    if (r < 0){
        fprintf(stderr,
                "Ошибка: дескриптор устройства не получен, код: %d.\n",r);
        return;
    }

    int ret = libusb_open(dev, &handle);
    if (ret < 0) {
        printf("Failed libusb_open: %d\n\r",ret);
        return;
    }

    int iSerialNumberSize = libusb_get_string_descriptor_ascii(handle, desc.iSerialNumber, uSerialNumber, sizeof(uSerialNumber));
    printf("%s \n", uSerialNumber);
    libusb_close(handle);
}

void printdev(libusb_device *dev){
    libusb_device_descriptor desc; // дескриптор устройства
    libusb_config_descriptor *config; // дескриптор конфигурации объекта

    int r = libusb_get_device_descriptor(dev, &desc);
    if (r < 0){
        fprintf(stderr,
                "Ошибка: дескриптор устройства не получен, код: %d.\n",r);
        return;
    }
    printf(" %.2X %.2X %.2X %.4d %.5d ",
            desc.bDeviceClass,
            desc.bDeviceSubClass,
            desc.bDeviceProtocol,
            desc.idVendor,
            desc.idProduct);
    printSerialNumber(dev);

}