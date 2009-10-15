
#include "keyfile.h"
#include "file.h"
#include "../util/macro.h"

uint8_t *_record(KEYFILE *kf, uint8_t type, size_t *rec_len);


uint8_t *_record(KEYFILE *kf, uint8_t type, size_t *rec_len)
{
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
