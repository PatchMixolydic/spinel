#include <stdbool.h>
#include <string.h>

static const char DigitsUpper[] = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ";
static const char DigitsLower[] = "0123456789abcdefghijklmnopqrstuvwxyz";
static const char BadBaseMessage[] = "[bad base]";
static const unsigned int MinBase = 2;
static const unsigned int MaxBase = 36;

/*
 * Handles base2-base36 gracefully
 * Most implementations I've seen can go further, but they can yield strange
 * results, since most of them use arithmetic with char (assuming ASCII):
 * [uppercase letters] 40 = _ in base 41
 * [lowercase letters] 40 = DEL in base 41(!!!)
 * Although it might break portability slightly if someone likes having numbers
 * that look like 346[^` or numbers that delete characters, I feel like this
 * might be saner...
 * bufSizeOrZero is bufSize if the buffer size should be constrained else 0
 * upper is used for printf family and signals if letters should be uppercase
 */
char* __itoa(int value, char buf[], size_t bufSizeOrZero, int base, bool isUnsigned, bool uppercase) {
    int i = 0;
    unsigned int valueAsUnsigned = value;
    const char* const Digits = uppercase ? DigitsUpper : DigitsLower;

    if ((base > MaxBase || base < MinBase) && bufSizeOrZero > strlen(BadBaseMessage)) {
        // TODO: strcpy
        memcpy(buf, BadBaseMessage, strlen(BadBaseMessage));
        return buf;
    }
    if (value == 0) {
        // algorithm can't detect 0 -- do it ourselves
        buf[i++] = '0';
        buf[i] = '\0';
        return buf;
    }

    // can only signed if the base is 10
    bool isNegative = !isUnsigned && base == 10 && value < 0;
    if (isNegative) {
        value = -value;
    }

    if (!isUnsigned) {
        while (value != 0 && (bufSizeOrZero == 0 || i < bufSizeOrZero)) {
            int bottomDigit = value % base;
            buf[i++] = Digits[bottomDigit];
            value /= base;
        }
    } else {
        while (valueAsUnsigned != 0 && (bufSizeOrZero == 0 || i < bufSizeOrZero)) {
            int bottomDigit = valueAsUnsigned % base;
            buf[i++] = Digits[bottomDigit];
            valueAsUnsigned /= base;
        }
    }

    if (isNegative) {
        buf[i++] = '-';
    }

    buf[i] = '\0';

    char temp;
    // the string is currently reversed; fix that
    for (int start = 0, end = i - 1; start < end; start++, end--) {
        temp = buf[start];
        buf[start] = buf[end];
        buf[end] = temp;
    }

    return buf;
}

char* itoa(int value, char buf[], int base) {
    return __itoa(value, buf, 0, base, false, true);
}

char* nitoa(int value, char buf[], size_t bufSize, int base) {
    return __itoa(value, buf, bufSize, base, false, true);
}

char* uitoa(int value, char buf[], int base) {
    return __itoa(value, buf, 0, base, true, true);
}

char* nuitoa(int value, char buf[], size_t bufSize, int base) {
    return __itoa(value, buf, bufSize, base, true, true);
}
