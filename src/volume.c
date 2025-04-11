// #include <dirent.h>
// #include <unistd.h>
// #include <sys/stat.h>
// #include <errno.h>
// #include <stdint.h>
// #include <string.h>

// #include "volume.h"
// #include "log.h"
// #include "common.h"
// #include "database.h"
// #include "crud.h"

// #define ARENA_IMPLEMENTATION

// __thread char volume_err[512];

// static VolumeList eVolList;
// static VolumeList mVolList;

// static VolumeConfig cfg;

// static void fifo_push(VolumeList *vol, VolumeEntry *entry)
// {
//     if (!vol->entries)
//     {
//         vol->entries = entry;
//         entry->next = entry->prev = entry;
//     }
//     else
//     {
//         VolumeEntry *tail = vol->entries->prev;
//         tail->next = entry;
//         entry->prev = tail;
//         entry->next = vol->entries;
//         vol->entries->prev = entry;
//     }

//     vol->total_size += entry->meta.size;
//     vol->entry_count++;
// }

// static void fifo_evict(VolumeList *vol)
// {
//     if (vol->entries)
//     {
//         VolumeEntry *to_evict = vol->entries;

//         if (to_evict->next == to_evict)
//         {
//             vol->entries = NULL;
//         }
//         else
//         {
//             vol->entries = to_evict->next;
//             to_evict->prev->next = to_evict->next;
//             to_evict->next->prev = to_evict->prev;
//         }

//         vol->total_size -= to_evict->meta.size;
//         vol->entry_count--;

//         // Actual file deletion
//         volume_delete(to_evict->impath);

//         free(to_evict);
//     }
// }

// // use the int fifo_delete(VolumeList *vol, int img_id)
// static int fifo_delete(VolumeList *vol, int img_id)
// {
//     if (!vol || !vol->entries)
//         return 0;

//     VolumeEntry *current = vol->entries;
//     do
//     {
//         if (current->img_id == img_id)
//         {
//             if (current->next == current)
//             {
//                 vol->entries = NULL;
//             }
//             else
//             {
//                 current->prev->next = current->next;
//                 current->next->prev = current->prev;
//                 if (current == vol->entries)
//                 {
//                     vol->entries = current->next;
//                 }
//             }

//             vol->total_size -= current->meta.size;
//             vol->entry_count--;

//             free(current);
//             return 1;
//         }
//         current = current->next;
//     } while (current != vol->entries);

//     return 0;
// }

// int volume_delete(const char *impath)
// {
//     if (!impath)
//         return 0;

//     int img_id = 0;
//     if (sscanf(impath + 7, "%d_", &img_id) != 1)
//     {
//         return 0;
//     }

//     // Handle memory volumes (mVol)
//     if (strncmp(impath, "mVol://", 7) == 0)
//     {
//         // Find the entry first to free its memory
//         VolumeEntry *current = mVolList.entries;
//         if (current)
//         {
//             do
//             {
//                 if (current->img_id == img_id)
//                 {
//                     // Free the memory buffer before deletion
//                     if (current->path.mem_ptr)
//                     {
//                         free(current->path.mem_ptr);
//                         current->path.mem_ptr = NULL;
//                     }
//                     break;
//                 }
//                 current = current->next;
//             } while (current != mVolList.entries);
//         }

//         // Then delete from tracking list
//         int result = fifo_delete(&mVolList, img_id);
//         return result;
//     }

//     // Handle disk volumes (pVol and eVol)
//     char volume_type[5] = {0};
//     strncpy(volume_type, impath, 4);

//     const char *base_dir = NULL;
//     VolumeList *vol = NULL;

//     if (strcmp(volume_type, "pVol") == 0)
//     {
//         base_dir = CONFIG_PVOL_DIRECTORY_PATH;
//     }
//     else if (strcmp(volume_type, "eVol") == 0)
//     {
//         base_dir = CONFIG_EVOL_DIRECTORY_PATH;
//         vol = &eVolList;
//     }
//     else
//     {
//         return 0;
//     }

//     char fs_path[PATH_BUFFER_SIZE];
//     snprintf(fs_path, sizeof(fs_path), "%s/%s", base_dir, impath + 7);

//     if (unlink(fs_path) != 0 && errno != ENOENT)
//     {
//         return 0;
//     }

//     // For eVol, remove from tracking list
//     if (vol)
//     {
//         int result = fifo_delete(vol, img_id);
//         return result;
//     }

//     return 1;
// }

// int volume_init(const VolumeConfig *vol_cfg)
// {
//     cfg = *vol_cfg;

//     // initialization for pVol
//     if (mkdir(cfg.pVolDirPath, 0755) == -1 && errno != EEXIST)
//     {
//         snprintf(volume_err, PATH_BUFFER_SIZE,
//                  "Failed to create pVol directory: %s", strerror(errno));
//         return 0;
//     }

//     // initialization for eVol
//     eVolList = (VolumeList){
//         .entries = NULL,
//         .total_size = 0,
//         .entry_count = 0,
//         .last_id = 0,
//     };

//     if (mkdir(cfg.eVolDirPath, 0755) == -1 && errno != EEXIST)
//     {
//         snprintf(volume_err, PATH_BUFFER_SIZE,
//                  "Failed to create eVol directory: %s", strerror(errno));
//         return 0;
//     }

//     // initializeation for mVol
//     mVolList = (VolumeList){
//         .entries = NULL,
//         .total_size = 0,
//         .entry_count = 0,
//         .last_id = 0,
//     };

//     return 1;
// }

// void volume_cleanup()
// {
//     // Clear ephemeral volume (eVol)
//     VolumeEntry *current = eVolList.entries;
//     while (current != NULL)
//     {
//         VolumeEntry *next = current->next;

//         free(current);
//         current = next;
//     }
//     eVolList.entries = NULL;
//     eVolList.total_size = 0;
//     eVolList.entry_count = 0;
//     eVolList.last_id = 0;

//     // Clear memory volume (mVol)
//     current = mVolList.entries;
//     while (current != NULL)
//     {
//         VolumeEntry *next = current->next;

//         if (current->path.mem_ptr != NULL)
//         {
//             free(current->path.mem_ptr);
//         }

//         free(current);
//         current = next;
//     }
//     mVolList.entries = NULL;
//     mVolList.total_size = 0;
//     mVolList.entry_count = 0;
//     mVolList.last_id = 0;
// }

// int volume_mkimpath(VolumeType type, const char *filename, int image_id, char *dest)
// {
//     if (!filename || !dest)
//     {
//         if (dest)
//             strncpy(dest, "error://invalid_args", PATH_BUFFER_SIZE);
//         return 0;
//     }

//     const char *vol_prefix;
//     switch (type)
//     {
//     case VOL_TYPE_PERSISTENT:
//         vol_prefix = "pVol";
//         break;
//     case VOL_TYPE_EPHEMERAL:
//         vol_prefix = "eVol";
//         break;
//     case VOL_TYPE_MEMORY:
//         vol_prefix = "mVol";
//         break;
//     default:
//         snprintf(dest, PATH_BUFFER_SIZE, "error://invalid_vol_type/%d_%s",
//                  image_id, filename);
//         return 0;
//     }

//     int written = snprintf(dest, PATH_BUFFER_SIZE, "%s://%d_%s",
//                            vol_prefix, image_id, filename);

//     if (written >= PATH_BUFFER_SIZE)
//     {
//         strncpy(dest, "error://path_too_long", PATH_BUFFER_SIZE);
//         return 0;
//     }

//     return 1;
// }

// static int volume_pVolwrite(const Image *img, int64_t original_id, const char *processing_params, sqlite3 *db)
// {
//     int64_t next_id;
//     if (db_get_next_image_id(db, &next_id) != SQLITE_OK)
//     {
//         return -1;
//     }

//     char dest_path[2*PATH_BUFFER_SIZE];
//     snprintf(dest_path, sizeof(dest_path), "%s/%d_%s",
//                            cfg.pVolDirPath, (int)next_id, img->meta.filename);

//     if (dump_image(img, dest_path) != 0)
//     {
//         return -1;
//     }

//     char impath[PATH_BUFFER_SIZE];
//     volume_mkimpath(VOL_TYPE_PERSISTENT, img->meta.filename, next_id, impath);

//     if (db_insert_image(db, &img->meta, impath, original_id, processing_params) != SQLITE_OK)
//     {
//         return -1;
//     }

//     return next_id;
// }

// static int volume_eVolwrite(const Image *img, int64_t original_id, const char *processing_params, sqlite3 *db)
// {
//     int64_t next_id;
//     if (db_get_next_image_id(db, &next_id) != SQLITE_OK)
//     {
//         return -1;
//     }

//     char impath[PATH_BUFFER_SIZE];
//     volume_mkimpath(VOL_TYPE_EPHEMERAL, img->meta.filename, next_id, impath);

//     while (eVolList.entry_count >= cfg.eVolImageNLimit ||
//            (eVolList.total_size + img->meta.size > cfg.eVolSizeLimit))
//     {
//         fifo_evict(&eVolList);
//     }

//     char dest_path[2*PATH_BUFFER_SIZE];
//     snprintf(dest_path, sizeof(dest_path), "%s/%d_%s", cfg.eVolDirPath, (int)next_id, img->meta.filename);

//     if (dump_image(img, dest_path) != 0)
//     {
//         return -1;
//     }

//     VolumeEntry *entry = calloc(1, sizeof(VolumeEntry));
//     if (!entry)
//     {
//         snprintf(volume_err, PATH_BUFFER_SIZE, "Memory allocation failed");
//         return -1;
//     }

//     entry->img_id = next_id;
//     strncpy(entry->path.disk, dest_path, PATH_BUFFER_SIZE);
//     strncpy(entry->impath, impath, PATH_BUFFER_SIZE);

//     if (db_insert_image(db, &img->meta, impath, original_id, processing_params) != SQLITE_OK)
//     {
//         free(entry);
//         return -1;
//     }

//     fifo_push(&eVolList, entry);
//     return next_id;
// }

// static int volume_mVolWrite(const Image *img, int64_t original_id, const char *processing_params, sqlite3 *db)
// {
//     int64_t next_id;
//     if (db_get_next_image_id(db, &next_id) != SQLITE_OK)
//     {
//         snprintf(volume_err, PATH_BUFFER_SIZE, "Failed to get next ID");
//         return -1;
//     }

//     while (mVolList.entry_count >= cfg.mVolImageNLimit ||
//            (mVolList.total_size + img->meta.size > cfg.mVolSizeLimit))
//     {
//         fifo_evict(&mVolList);
//     }

//     void *img_data = malloc(img->meta.size);
//     if (!img_data)
//     {
//         snprintf(volume_err, PATH_BUFFER_SIZE, "Memory allocation failed");
//         return -1;
//     }

//     memcpy(img_data, img->data, img->meta.size);

//     char impath[PATH_BUFFER_SIZE];
//     volume_mkimpath(VOL_TYPE_MEMORY, img->meta.filename, next_id, impath);

//     VolumeEntry *entry = calloc(1, sizeof(VolumeEntry));
//     if (!entry)
//     {
//         free(img_data);
//         snprintf(volume_err, PATH_BUFFER_SIZE, "Entry allocation failed");
//         return -1;
//     }

//     entry->img_id = next_id;
//     entry->path.mem_ptr = img_data;
//     strncpy(entry->impath, impath, PATH_BUFFER_SIZE);

//     if (db_insert_image(db, &img->meta, impath, original_id, processing_params) != SQLITE_OK)
//     {
//         free(img_data);
//         free(entry);
//         snprintf(volume_err, PATH_BUFFER_SIZE, "Database insert failed");
//         return -1;
//     }

//     fifo_push(&mVolList, entry);
//     return next_id;
// }

// int volume_write(VolumeType type, const Image *img, int64_t original_id, const char *processing_params, sqlite3 *db)
// {
//     switch (type)
//     {
//     case VOL_TYPE_PERSISTENT:
//         return volume_pVolwrite(img, original_id, processing_params, db);
//         break;
//     case VOL_TYPE_EPHEMERAL:
//         return volume_eVolwrite(img, original_id, processing_params, db);
//     case VOL_TYPE_MEMORY:
//         return volume_mVolWrite(img, original_id, processing_params, db);
//     default:
//         return -1;
//     }
// }