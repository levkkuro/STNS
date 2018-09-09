#include "stns.h"

static json_t *entries      = NULL;
static int entry_idx        = 0;
pthread_mutex_t grent_mutex = PTHREAD_MUTEX_INITIALIZER;

#define GROUP_ENSURE(entry)                                                                                            \
  const json_int_t id = json_integer_value(json_object_get(entry, "id"));                                              \
  const char *name    = json_string_value(json_object_get(entry, "name"));                                             \
  char passwd[]       = "x";                                                                                           \
                                                                                                                       \
  printf("name=%s id=%d\n", name, (int)id);                                                                            \
  if (name == NULL) {                                                                                                  \
    pthread_mutex_unlock(&grent_mutex);                                                                                \
    return NSS_STATUS_NOTFOUND;                                                                                        \
  }                                                                                                                    \
  rbuf->gr_gid = c->gid_shift + id;                                                                                    \
                                                                                                                       \
  SET_ATTRBUTE(gr, name, name)                                                                                         \
  SET_ATTRBUTE(gr, passwd, passwd)                                                                                     \
  rbuf->gr_mem = (char **)buf;                                                                                         \
                                                                                                                       \
  json_t *members = json_object_get(entry, "users");                                                                   \
  int i;                                                                                                               \
  int ptr_area_size = (json_array_size(members) + 1) * sizeof(char *);                                                 \
  char *next_member;                                                                                                   \
                                                                                                                       \
  if (buflen < ptr_area_size) {                                                                                        \
    (*errnop) = ERANGE;                                                                                                \
    pthread_mutex_unlock(&grent_mutex);                                                                                \
    return NSS_STATUS_TRYAGAIN;                                                                                        \
  }                                                                                                                    \
                                                                                                                       \
  next_member = buf + ptr_area_size;                                                                                   \
  for (i = 0; i < json_array_size(members); i++) {                                                                     \
    json_t *member = json_array_get(members, i);                                                                       \
    if (!json_is_string(member)) {                                                                                     \
      pthread_mutex_unlock(&grent_mutex);                                                                              \
      return NSS_STATUS_UNAVAIL;                                                                                       \
    }                                                                                                                  \
    const char *user = json_string_value(member);                                                                      \
    int user_length  = strlen(user) + 1;                                                                               \
    if (buflen < user_length) {                                                                                        \
      *errnop = ERANGE;                                                                                                \
      pthread_mutex_unlock(&grent_mutex);                                                                              \
      return NSS_STATUS_TRYAGAIN;                                                                                      \
    }                                                                                                                  \
    strcpy(next_member, user);                                                                                         \
    rbuf->gr_mem[i] = next_member;                                                                                     \
    next_member += user_length + 1;                                                                                    \
    buflen -= user_length + 1;                                                                                         \
  }                                                                                                                    \
  rbuf->gr_mem[json_array_size(members)] = NULL;

STNS_ENSURE_BY(name, const char *, group_name, string, name, (strcmp(current, group_name) == 0), group, GROUP)
STNS_ENSURE_BY(gid, gid_t, gid, integer, id, current == gid, group, GROUP)

STNS_GET_SINGLE_VALUE_METHOD(getgrnam_r, const char *name, "groups?name=%s", name, group, )
STNS_GET_SINGLE_VALUE_METHOD(getgrgid_r, gid_t gid, "groups?id=%d", gid, group, GROUP_ID_QUERY_AVAILABLE)
STNS_SET_ENTRIES(gr, GROUP, group, groups)
