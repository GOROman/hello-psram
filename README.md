# hello-psram

#PSRAM入門

## 概要

ESP32シリーズで使われる PSRAM (Pseudo Static Random Access Memory)。
我々捜査班はPSRAMの謎に迫るべくリポジトリを作成した...。

## PSRAM とは？

ESP32の場合は、SPI接続された外部メモリ。ESP32-S3 によってはOn-Chipだったりする。

## PSRAM の有効化

~~~
idf.py menuconfig
~~~

- Component Config → ESP PSRAM → Support for external, SPI-connected RAM を有効にする。

- 参考動画: https://x.com/GOROman/status/1873920990851596765

### Quad or Octal

PSRAMの容量などで接続方法が違う。

- Quad: 信号線4本
- Octal: 信号線8本

こちらも menuconfig で設定する。間違えると起動時に怒られる。



## ビルド方法

~~~
idf.py build
~~~

## 実行方法

~~~
idf.py flash monitor
~~~

## 参考資料

- Support for External RAM ( https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-guides/external-ram.html )

