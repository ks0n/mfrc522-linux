#ifndef MFRC522_H
#define MFRC522_H

int mfrc522_read_rs(void); // FIXME
int mfrc522_write_rs(const char *buffer, size_t len);

#endif /* !MFRC522_H */
