#include "org_videolan_Libbluray.h"

#include "libbluray/bdj/bdj.h"
#include "libbluray/bdj/bdj_private.h"
#include "libbluray/register.h"

JNIEXPORT jint JNICALL Java_org_videolan_Libbluray_getCurrentTitleNumN
  (JNIEnv * env, jclass cls, jlong pBDJ)
{
    return 0;
}

JNIEXPORT jint JNICALL Java_org_videolan_Libbluray_writeGPRN
  (JNIEnv * env, jclass cls, jlong pBDJ, jint num, jint value)
{
    BDJAVA* bdj = (BDJAVA*)pBDJ;
    return bd_gpr_write(bdj->reg, num, value);
}

JNIEXPORT jint JNICALL Java_org_videolan_Libbluray_readGPRN
  (JNIEnv * env, jclass cls, jlong pBDJ, jint num)
{
    BDJAVA* bdj = (BDJAVA*)pBDJ;
    return bd_gpr_read(bdj->reg, num);
}

JNIEXPORT jint JNICALL Java_org_videolan_Libbluray_readPSRN
  (JNIEnv * env, jclass cls, jlong pBDJ, jint num)
{
    BDJAVA* bdj = (BDJAVA*)pBDJ;
    return bd_psr_read(bdj->reg, num);
}
