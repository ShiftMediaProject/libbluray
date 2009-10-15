
#include "keyfile.h"
#include "file.h"
#include "../util/macro.h"

uint8_t *keyfile_record(KEYFILE *kf, enum keyfile_types type, uint16_t *entries, size_t *entry_len)
{
    size_t pos = 0, len = 0;

    while (pos + 4 <= len) {
        len = MKINT_BE24(kf->buf + pos + 1);

        if (entries) {
            *entries = MKINT_BE16(kf->buf + pos + 4);
        }

        if (entry_len) {
            *entry_len = MKINT_BE32(kf->buf + pos + 6);
        }

        if (kf->buf[pos] == type)
            return kf->buf + pos + 10;  // only return ptr to first byte of entry

        pos += len;
    }

    return NULL;
}

KEYFILE *keyfile_open(const char *path)
{
    FILE_H *fp = NULL;
    KEYFILE *kf = malloc(sizeof(KEYFILE));

    if ((fp = file_open(path, "rb"))) {
        file_seek(fp, 0, SEEK_END);
        kf->size = file_tell(fp);
        file_seek(fp, 0, SEEK_SET);

        kf->buf = malloc(kf->size);

        file_read(fp, kf->buf, kf->size);

        file_close(fp);

        X_FREE(fp);

        return kf;
    }

    return NULL;
}

void keyfile_close(KEYFILE *kf)
{
    X_FREE(kf->buf);
    X_FREE(kf);
}
