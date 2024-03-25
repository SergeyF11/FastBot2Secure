# FastBot2 [beta]
Очень быстрая и лёгкая библиотека Telegram бота для Arduino, ESP8266, ESP32

> Библиотека в разработке! Руководствуйтесь примером и подсказками IDE

## Установка
Библиотека не в релизе, качайте репозиторий zip архивом (кнопка Code справа вверху) или по [прямой ссылке](https://github.com/GyverLibs/FastBot2/archive/refs/heads/main.zip) на загрузку.

Для работы FastBot2 нужно установить библиотеки:
- [GSON](https://github.com/GyverLibs/GSON)
- [StringUtils](https://github.com/GyverLibs/StringUtils)

## Режим опроса обновлений
### Sync (по умолчанию)
```cpp
bot.setPollMode(FastBot2::Poll::Sync, 4000);
```
#### Обновление
Такой же режим, как в первой версии: библиотека запрашивает обновления и ждёт ответа внутри `tick()`. При плохой связи может "зависать" внутри `tick()` на ощутимое (не дольше таймаута) время. Ставить период меньше 4000 не рекомендуется - сервер Телеграм "накажет" долгим ответом за слишком частый опрос!

#### Отправка сообщений
Отправка сообщений будет быстрой в любом месте программы, т.к. библиотека не ждёт ответа от сервера.

### Async
```cpp
bot.setPollMode(FastBot2::Poll::Async, 4000);
```
#### Обновление
"Асинхронный" режим - библиотека запросит обновления и обработает ответ, когда он придёт. Период опроса - такая же особенность, как в `Sync`.

#### Отправка сообщений
Если отправить сообщение НЕ в обработчике и ВО ВРЕМЯ ожидания ответа с обновлением - библиотека переподключится к серверу, что занимает около 1 секунды и блокирует выполнение программы. Для безопасной отправки из любого места в программе нужно отправлять сообщения, когда библиотека не ждёт ответа - проверить можно по флагу `waiting()`.

### Long
```cpp
bot.setPollMode(FastBot2::Poll::Long, 10000);
```
#### Обновление
Самый правильный способ опроса - асинхронный long polling. Период можно ставить побольше - от 10 секунд. Библиотека отправляет запрос и получит ответ либо по указанному таймауту, либо по факту поступления новых сообщений в бота. Таймаут переподключения - по умолчанию 300мс. Таким образом это самый быстрый и безопасный способ получения обновлений. Но есть но.

#### Отправка сообщений
Отправка сообщений в обработчике обновлений - безопасна, сообщения будут отправлены быстро. Если нужно отправить из другого места программы - по флагу `waiting()` можно узнать, ждёт ли библиотека ответ сервера. Если отправить пока ждёт - будет переподключение ("зависнет" на ~1 секунду).


## Отправка сообщений
### Асинхронность и ID
ID отправленного ботом сообщения можно получить из `lastBotMessage()`. Нужно понимать, что у каждого чата свой счётчик ID сообщений. Библиотека получает ID отправленного сообщения из ответа сервера, поэтому есть несколько важных моментов.

Сообщение может быть отправлено двумя способами:
- **Синхронно** - библиотека ждёт и обрабатывает ответ сервера внутри отправки сообщения, поэтому сразу после вызова `sendMessage` функция `lastBotMessage()` возвращает актуальный ID отправленного сообщения
- **Асинхронно** (по умолчанию) - библиотека не ждёт ответа от сервера и получит его в следующем `tick()`, т.е. `lastBotMessage()` не выдаст достоверный ID сразу после отправки

Поэтому в библиотеке есть несколько условностей и особенностей:
- Принудительно отправить сообщение с ожиданием ответа (синхронно) можно поставив флаг `wait` - `sendMessage(m, true)`
- При отправке нескольких асинхронных сообщений подряд библиотека принудительно будет ждать ответ сервера для каждого предыдущего сообщения, т.е. сделает их синхронными, чтобы избежать переподключения к серверу

```cpp
fb::Message msg("hello", 1234567);
bot.sendMessage(msg);               // асинхронно
Serial.println(lastBotMessage());   // ID может быть некорректным

bot.sendMessage(msg, true);         // синхронно
// библиотека дождётся ответа от предыдущего сообщения, и от нового
Serial.println(lastBotMessage());   // реальный ID
```
