# hello-psram

公式ハッシュタグ: #PSRAM入門

## 概要

ESP32シリーズで使われる PSRAM (Pseudo Static Random Access Memory)。
我々捜査班はPSRAMの謎に迫るべくリポジトリを作成した...。

## PSRAM とは？

ESP32の場合は、SPI接続された外部メモリ。ESP32-S3 によってはOn-Chipだったりする。

## PSRAM の有効化

まずは menuconfig する。

~~~
idf.py menuconfig
~~~

- Component Config → ESP PSRAM → Support for external, SPI-connected RAM を有効にする。

<img width="548" alt="image" src="https://github.com/user-attachments/assets/a52d841f-d0c9-4aa1-aa0a-b8ea9beae9f1" />

- 参考動画: https://x.com/GOROman/status/1873920990851596765

### Quad or Octal

PSRAMの容量などで接続方法(```Quad``` ```Octal```)が違う。

- ```Quad```: 信号線4本
- ```Octal```: 信号線8本

こちらも menuconfig で設定する。間違えると起動時に怒られる。

<img width="548" alt="image" src="https://github.com/user-attachments/assets/34fa2de7-66f2-4ef0-95a7-8ecf9d28f964" />

- ATOMS3R(ESP32-S3-PICO-1-N8R8) の場合は、```Octal```。

<img width="685" alt="image" src="https://github.com/user-attachments/assets/5c58a366-e8d8-4a75-a31a-c5139516afef" />


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

