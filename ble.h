#ifndef _BLE_H
#define _BLE_H

class ble {
  public:
    ble();
    bool connected();
    void disable();
    bool enabled();
    void loop();
    void start();
    
  private:
    String _ipAddress;
    String _ssId;
};

extern ble _ble;

#endif