/*
 * This file is part of libbluray
 * Copyright (C) 2010-2014  Petri Hintukainen <phintuka@users.sourceforge.net>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library. If not, see
 * <http://www.gnu.org/licenses/>.
 */

#if HAVE_CONFIG_H
#include "config.h"
#endif

#include "mutex.h"

#include "logging.h"
#include "macro.h"


#if defined(_WIN32)
  /* nothing here */
#elif defined(HAVE_PTHREAD_H)

#include <pthread.h>

int bd_mutex_init(BD_MUTEX *p)
{
    p->owner      = (pthread_t)-1;
    p->lock_count = 0;

    if (pthread_mutex_init(&p->mutex, NULL)) {
        BD_DEBUG(DBG_BLURAY|DBG_CRIT, "bd_mutex_init() failed !\n");
        return -1;
    }

    return 0;
}

int bd_mutex_destroy(BD_MUTEX *p)
{
    bd_mutex_lock(p);
    bd_mutex_unlock(p);
    if (pthread_mutex_destroy(&p->mutex)) {
        BD_DEBUG(DBG_BLURAY|DBG_CRIT, "bd_mutex_destroy() failed !\n");
        return -1;
    }
    return 0;
}

#endif /* HAVE_PTHREAD_H */
