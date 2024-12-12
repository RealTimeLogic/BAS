
/* Example code showing how to set the mount point, if required
 */

#include "xedge.h"

int xedgeInitDiskIo(DiskIo* dio)
{
   DiskIo_setRootDir(dio,"/your/mount/point");
   return 0; 
}
