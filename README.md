# Sistem Peringatan Dini Kebakaran Berbasis IoT

Projek ini merupakan purwarupa sistem keamanan cerdas yang dirancang untuk mendeteksi indikasi kebakaran secara dini. Dengan memanfaatkan protokol **MQTT (Message Queuing Telemetry Transport)** untuk efisiensi pengiriman data ke Blynk dan **Telegram Bot API** sebagai media notifikasi instan.

## 👥 Anggota Kelompok
* **Khairil Syukri** - 2309106031
* **Akmal Alvian Pratama** - 2309106021
* **Vicky Ahmad Fernanda** - 2309106123

## 📝 Deskripsi Projek
Sistem ini bekerja secara kontinu untuk memantau tiga parameter utama lingkungan: suhu udara, keberadaan asap/gas mudah terbakar, dan munculnya titik api.

**Mekanisme Kerja:**
1.  **Deteksi:** Sensor MQ-2, DHT22, dan Flame Sensor mengumpulkan data lingkungan.
2.  **Transmisi:** Data dikirimkan ke cloud server Blynk menggunakan protokol MQTT untuk monitoring visual pada smartphone.
3.  **Notifikasi:** Jika suhu > 50°C, asap > 1800 (ADC), atau sensor api mendeteksi gelombang infrared, sistem akan:
    * Mengaktifkan Buzzer dan LED secara lokal.
    * Mengirim pesan peringatan bahaya ke grup/kontak Telegram pengguna.
4.  **Kendali Jarak Jauh:** Pengguna dapat mematikan atau mengaktifkan mode pemantauan melalui tombol virtual di dashboard Blynk.

## 🛠️ Komponen yang Digunakan
* **ESP32 DevKit V1** (Microcontroller Utama)
* **DHT22** (Sensor Suhu & Kelembapan)
* **MQ-2** (Sensor Gas & Asap)
* **Flame Sensor** (Sensor Deteksi Api)
* **2x Active Buzzer** & **LED Merah** (Output Alarm)

## 🏗️ Pembagian Tugas
* **Akmal Alvian Pratama**: Implementasi firmware ESP32, konfigurasi Broker MQTT Blynk, integrasi API Telegram, dan pengujian fungsionalitas sistem.
* **[Nama Anggota 2]**: Perancangan skematik rangkaian (Fritzing), perakitan hardware, dan penyusunan dokumentasi laporan.

## 📐 Board Schematic
Berikut adalah skema rangkaian sistem yang dirancang menggunakan Cirkit Designer:

![Board Schematic](./circuit_image.jpg)

| Komponen | Pin ESP32 | Jenis Pin |
| :--- | :--- | :--- |
| **MQ-2 Gas** | GPIO 34 | Analog Input |
| **Flame Sensor** | GPIO 25 | Digital Input |
| **DHT22** | GPIO 13 | Digital (One-Wire) |
| **Buzzer 1 & 2** | GPIO 26 & 27 | Digital Output |
| **LED Red** | GPIO 5 | Digital Output |
