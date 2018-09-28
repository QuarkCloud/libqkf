
#include "qkf/field.h"
#include "qkf/malloc.h"
#include <string.h>

qkf_field_str_t * qkf_field_str_new(const char * str)
{
    size_t slen = 0 ;
    if(str != NULL)
        slen = ::strlen(str) ;
    if(slen == 0)
        return NULL ;

    if(slen >= INT16_MAX)
        slen = INT16_MAX ;

    size_t size = slen + 1 + 2 ;
    qkf_field_str_t * fstr = (qkf_field_str_t *)::qkf_malloc(kDefaultMMgr , size) ;
    if(fstr == NULL)
        return NULL ;
    fstr->size = (uint16_t)slen ;
    ::memcpy(fstr->data , str , slen) ;
    fstr->data[slen] = '\0' ;
    return fstr ;
}

qkf_field_raw_t * qkf_field_raw_new(const void * raw , size_t size)
{
    if(raw == NULL || size == 0)
        return NULL ;

    size_t fsize = size + 4 ;
    qkf_field_raw_t * fraw = (qkf_field_raw_t *)::qkf_malloc(kDefaultMMgr , fsize) ;
    if(fraw == NULL)
        return NULL ;
    fraw->size = (uint32_t)size ;
    ::memcpy(fraw->data , raw , size) ;
    return fraw ;
}


bool qkf_field_data_set_bool(qkf_field_data_t * field , bool value)
{
    if(field == NULL)
        return false ;
    field->bv = value ;
    return true ;
}

bool qkf_field_data_set_time(qkf_field_data_t * field , time_t value)
{
    if(field == NULL)
        return false ;
    field->ts = value ;
    return true ;
}

bool qkf_field_data_set_int8(qkf_field_data_t * field , int8_t value)
{
    if(field == NULL)
        return false ;
    field->i8 = value ;
    return true ;
}

bool qkf_field_data_set_uint8(qkf_field_data_t * field , uint8_t value)
{
    if(field == NULL)
        return false ;
    field->u8 = value ;
    return true ;
}

bool qkf_field_data_set_int16(qkf_field_data_t * field , int16_t value)
{
    if(field == NULL)
        return false ;
    field->i16 = value ;
    return true ;
}

bool qkf_field_data_set_uint16(qkf_field_data_t * field , uint16_t value)
{
    if(field == NULL)
        return false ;
    field->u16 = value ;
    return true ;
}

bool qkf_field_data_set_int32(qkf_field_data_t * field , int32_t value)
{
    if(field == NULL)
        return false ;
    field->i32 = value ;
    return true ;
}

bool qkf_field_data_set_uint32(qkf_field_data_t * field , uint32_t value)
{
    if(field == NULL)
        return false ;
    field->u32 = value ;
    return true ;
}

bool qkf_field_data_set_int64(qkf_field_data_t * field , int64_t value)
{
    if(field == NULL)
        return false ;
    field->i64 = value ;
    return true ;
}

bool qkf_field_data_set_uint64(qkf_field_data_t * field , uint64_t value)
{
    if(field == NULL)
        return false ;
    field->u64 = value ;
    return true ;
}

bool qkf_field_data_set_float(qkf_field_data_t * field , float value)
{
    if(field == NULL)
        return false ;
    field->f32 = value ;
    return true ;
}

bool qkf_field_data_set_double(qkf_field_data_t * field , double value)
{
    if(field == NULL)
        return false ;
    field->f64 = value ;
    return true ;
}

bool qkf_field_data_set_str(qkf_field_data_t * field , const char * str)
{
    if(field == NULL)
        return false;
    if(field->str != NULL)
        ::qkf_free(kDefaultMMgr , field->str) ;
    field->str = qkf_field_str_new(str) ;
    return true ;
}

bool qkf_field_data_set_raw(qkf_field_data_t * field , const void * raw , size_t size)
{
    if(field == NULL)
        return false;
    if(field->raw != NULL)
        ::qkf_free(kDefaultMMgr , field->raw) ;
    field->raw = qkf_field_raw_new(raw , size) ;
    return true ;
}

bool qkf_field_data_set_list(qkf_field_data_t * field , const qkf_field_list_t *list)
{
    if(field == NULL)
        return false;
    if(field->list != NULL)
    {
        qkf_field_list_free(field->list) ;
        field->list = NULL ;
    }
    if(list == NULL)
        return true;

    qkf_field_list_t * new_list = qkf_field_list_new(list->type , list->capacity) ;
    if(new_list == NULL)
        return false;

    qkf_field_list_copy(list , new_list) ;
    field->list = new_list ;
    return true ;
}

bool qkf_field_data_set_map(qkf_field_data_t * field , const qkf_field_map_t * map)
{
    if(field == NULL)
        return false;
    if(field->map != NULL)
    {
        qkf_field_map_free(field->map) ;
        field->map = NULL ;
    }
    if(map == NULL)
        return true;

    qkf_field_map_t * new_map = qkf_field_map_new() ;
    if(new_map == NULL)
        return false;
    qkf_field_map_copy(map , new_map) ;
    field->map = new_map ;
    return true ;
}

bool qkf_field_data_attach_str(qkf_field_data_t * field , qkf_field_str_t * str)
{
    if(field == NULL)
        return false ;
    if(field->str != NULL)
        ::qkf_free(kDefaultMMgr , field->str) ;
    field->str = str ;
    return true ;
}

bool qkf_field_data_attach_raw(qkf_field_data_t * field , qkf_field_raw_t * raw)
{
    if(field == NULL)
        return false ;
    if(field->raw != NULL)
        ::qkf_free(kDefaultMMgr , field->raw) ;
    field->raw = raw ;
    return true ;
}

bool qkf_field_data_attach_list(qkf_field_data_t * field , qkf_field_list_t *list)
{
    if(field == NULL)
        return false ;
    if(field->list != NULL)
        qkf_field_list_free(field->list) ;
    field->list = list ;
    return true ;
}

bool qkf_field_data_attach_map(qkf_field_data_t * field , qkf_field_map_t * map)
{
    if(field == NULL)
        return false ;
    if(field->map != NULL)
        ::qkf_field_map_free(field->map) ;
    field->map = map ;
    return true ;
}

bool qkf_field_data_get_bool(qkf_field_data_t * field , bool& value)
{
    if(field == NULL)
        return false ;
    value = field->bv ;
    return true ;
}

bool qkf_field_data_get_time(qkf_field_data_t * field , time_t& value)
{
    if(field == NULL)
        return false ;
    value = field->ts ;
    return true ;
}

bool qkf_field_data_get_int8(qkf_field_data_t * field , int8_t& value)
{
    if(field == NULL)
        return false ;
    value = field->i8 ;
    return true ;
}

bool qkf_field_data_get_uint8(qkf_field_data_t * field , uint8_t& value)
{
    if(field == NULL)
        return false ;
    value = field->u8 ;
    return true ;
}

bool qkf_field_data_get_int16(qkf_field_data_t * field , int16_t& value)
{
    if(field == NULL)
        return false ;
    value = field->i16 ;
    return true ;
}

bool qkf_field_data_get_uint16(qkf_field_data_t * field , uint16_t& value)
{
    if(field == NULL)
        return false ;
    value = field->u16 ;
    return true ;
}

bool qkf_field_data_get_int32(qkf_field_data_t * field , int32_t& value)
{
    if(field == NULL)
        return false ;
    value = field->i32 ;
    return true ;
}

bool qkf_field_data_get_uint32(qkf_field_data_t * field , uint32_t& value)
{
    if(field == NULL)
        return false ;
    value = field->u32 ;
    return true ;
}

bool qkf_field_data_get_int64(qkf_field_data_t * field , int64_t& value)
{
    if(field == NULL)
        return false ;
    value = field->i64 ;
    return true ;
}

bool qkf_field_data_get_uint64(qkf_field_data_t * field , uint64_t& value)
{
    if(field == NULL)
        return false ;
    value = field->u64 ;
    return true ;
}

bool qkf_field_data_get_float(qkf_field_data_t * field , float& value)
{
    if(field == NULL)
        return false ;
    value = field->f32 ;
    return true ;
}

bool qkf_field_data_get_double(qkf_field_data_t * field , double& value)
{
    if(field == NULL)
        return false ;
    value = field->f64 ;
    return true ;
}

bool qkf_field_data_get_str(qkf_field_data_t * field , qkf_field_str_t *& str)
{
    if(field == NULL)
        return false ;
    str = field->str ;
    return true ;
}

bool qkf_field_data_get_raw(qkf_field_data_t * field , qkf_field_raw_t *& raw)
{
    if(field == NULL)
        return false ;
    raw = field->raw ;
    return true ;
}

bool qkf_field_data_get_list(qkf_field_data_t * field , qkf_field_list_t *&list)
{
    if(field == NULL)
        return false ;
    list = field->list ;
    return true ;
}

bool qkf_field_data_get_map(qkf_field_data_t * field , qkf_field_map_t *& map)
{
    if(field == NULL)
        return false;
    map = field->map ;
    return true ;
}

void qkf_field_data_free(uint8_t type , qkf_field_data_t * field)
{
    if(type <= kTypeMIN || type > kTypeMAX || field == NULL)
        return ;

    if(type == kTypeSTR)
    {
        if(field->str != NULL)
            ::qkf_free(kDefaultMMgr , field->str) ;
    }
    else if(type == kTypeRAW)
    {
        if(field->raw != NULL)
            ::qkf_free(kDefaultMMgr , field->raw) ;
    }
    else if(type == kTypeLIST)
    {
        if(field->list != NULL)
            ::qkf_field_list_free(field->list) ;
    }
    else if(type == kTypeMAP)
    {
        if(field->map != NULL)
            ::qkf_field_map_free(field->map) ;
    }

    field->val = 0 ;
}

bool qkf_field_data_copy(uint8_t type , const qkf_field_data_t * src , qkf_field_data_t * dst) 
{
    if(type == kTypeVOID || src == NULL || dst == NULL)
        return false ;

    dst->val = 0 ;
    if(type == kTypeSTR)
    {
        if(src->str != NULL)
            dst->str = ::qkf_field_str_new(src->str->data) ;
    }
    else if(type == kTypeRAW)
    {
        if(src->raw != NULL)
            dst->raw = ::qkf_field_raw_new(src->raw->data , src->raw->size) ;
    }
    else if(type == kTypeLIST)
    {
        if(src->list != NULL)
        {
            dst->list = ::qkf_field_list_new(src->list->type , 0) ;
            return qkf_field_list_copy(src->list , dst->list) ;
        }
    }
    else if(type == kTypeMAP)
    {
        if(src->map != NULL)
        {
            dst->map = ::qkf_field_map_new() ;
            return ::qkf_field_map_copy(src->map , dst->map) ;
        }
    }
    else
    {
        dst->val = src->val ;
    }

    return true ;
}

qkf_field_def_t * qkf_field_def_new(const char * name , uint8_t type)
{
    size_t fsize = sizeof(qkf_field_def_t) ;
    qkf_field_def_t * def = (qkf_field_def_t *)::qkf_malloc(kDefaultMMgr , fsize);
    if(def == NULL)
        return NULL ;

    if(qkf_field_def_init(def , name , type) == false)
    {
        ::qkf_free(kDefaultMMgr , def) ;
        return NULL ;
    }

    return def ;
}

void qkf_field_def_free(qkf_field_def_t * def)
{
    if(def == NULL)
        return ;

    qkf_field_def_final(def) ;
    ::qkf_free(kDefaultMMgr , def) ;
}

bool qkf_field_def_init(qkf_field_def_t * def , const char * name , uint8_t type)
{
    if(def == NULL || name == NULL || type < kTypeMIN || type > kTypeMAX)
        return false ;
    ::memset(def , 0 , sizeof(qkf_field_def_t)) ;

    size_t nsize = ::strlen(name) ;
    if(nsize == 0)
        return false ;

    def->name = ::qkf_strdup(name , ::strlen(name)) ;
    def->type = type ;
    return (def->name != NULL) ;
}

void qkf_field_def_final(qkf_field_def_t * def)
{
    if(def == NULL || def->name == NULL)
        return ;

    ::qkf_free(kDefaultMMgr , def->name) ;
    def->name = NULL ;
}


qkf_field_list_t * qkf_field_list_new(uint8_t type , int capacity)
{
    size_t lsize = sizeof(qkf_field_list_t) ;
    qkf_field_list_t * list = (qkf_field_list_t *)::qkf_malloc(kDefaultMMgr , lsize) ;
    if(list == NULL)
        return false ;

    if(qkf_field_list_init(list , type , capacity) == true)
        return list ;

    ::qkf_free(kDefaultMMgr , list) ;
    return false ;
}

bool qkf_field_list_init(qkf_field_list_t * list , uint8_t type , int capacity)
{
    if(list == NULL || capacity < 0)
        return false;
    ::memset(list , 0 , sizeof(qkf_field_list_t)) ;

    qkf_field_data_t * datas = NULL ;
    size_t data_size = sizeof(qkf_field_data_t) * capacity ;
    if(data_size != 0)
    {
        datas = (qkf_field_data_t *)::qkf_malloc(kDefaultMMgr , data_size) ;
        if(datas == NULL)
            return false ;
        ::memset(datas , 0 , data_size) ;
    }

    qkf_field_data_t * old_datas = list->datas ;
    list->datas = datas ;
    list->capacity = capacity ;
    list->type = type ;
    if(old_datas != NULL)
        ::qkf_free(kDefaultMMgr , old_datas) ;

    return true ;
}

void qkf_field_list_final(qkf_field_list_t * list)
{
    if(list == NULL)
        return ;

    qkf_field_data_t * datas = list->datas ;    
    if(datas != NULL)
    {
        uint8_t type = list->type ;
        if(type == kTypeSTR || type == kTypeRAW || type == kTypeLIST || type == kTypeMAP)
        {
            uint32_t head = list->head ;
            uint32_t tail = list->tail ;

            if(head < tail)
            {
                for(uint32_t fidx = head ; fidx < tail ; ++fidx)
                    qkf_field_data_free(type , datas + fidx) ;
            }
            else if(head > tail)
            {
                for(uint32_t fidx = head ; fidx < list->capacity ; ++fidx)
                    qkf_field_data_free(type , datas + fidx) ;

                for(uint32_t fidx = 0 ; fidx < tail ; ++fidx)
                    qkf_field_data_free(type , datas + fidx) ;            
            }
        }

        ::qkf_free(kDefaultMMgr , datas) ;
    }

    ::memset(list , 0 , sizeof(qkf_field_list_t)) ;
}

void qkf_field_list_free(qkf_field_list_t * list)
{
    if(list == NULL)
        return ;

    qkf_field_list_final(list) ;
    ::qkf_free(kDefaultMMgr , list) ;
}

void qkf_field_list_clear(qkf_field_list_t * list)
{
    if(list == NULL)
        return ;

    list->size = 0 ;
    list->head = 0 ;
    list->tail = 0 ;
}

bool qkf_field_list_push(qkf_field_list_t * list , qkf_field_data_t * data)
{
    if(list == NULL || data == NULL)
        return false ;

    if(list->size >= list->capacity)
    {
        if(qkf_field_list_extend(list) == false)
            return false ;
    }

    uint32_t tail = list->tail + 1;
    if(tail >= list->capacity)
        tail = 0 ;

    qkf_field_data_t * node = list->datas + tail ;
    node->val = data->val ;
    list->tail = tail ;
    return true ;
}

bool qkf_field_list_pop(qkf_field_list_t * list , qkf_field_data_t & data)
{
    if(list == NULL)
        return false ;

    if(list->size <= 0)
        return false ;

    uint32_t head = list->head + 1;
    if(head >= list->capacity)
        head = 0 ;

    qkf_field_data_t * node = list->datas + head ;
    data.val = node->val ;
    list->head = head ;

    qkf_field_list_shrink(list) ;
    return true ;
}
/**
    需要注意的是，copy_data只拷贝数组，没有检查数组的中值。
    如果该数组元素是指针类型，指针指向的内容没有被拷贝。
*/
bool qkf_field_list_copy_data(const qkf_field_list_t * list , qkf_field_data_t * datas) 
{
    if(list == NULL || datas == NULL)
        return false ;
    if(list->size == 0)
        return true ;

    //由于使用环形队列，拷贝数据同一般的不一样
    uint32_t head = list->head ;
    uint32_t tail = list->tail ;

    if(head < tail)
    {
        //head和tail之间是数据
        qkf_field_data_t * from = list->datas + head ;
        size_t size  = sizeof(qkf_field_data_t) * list->size ;
        if(size > 0)
            ::memcpy(datas , from , size) ;
    }
    else if(head > tail)
    {
        //0-->tail 和 head-->capacity之间是数据，分为两段
        qkf_field_data_t * from = list->datas + head ;
        uint32_t copy_count = list->capacity - head + 1 ;
        size_t size = sizeof(qkf_field_data_t) * (copy_count) ;
        ::memcpy(datas , from , size) ;

        from = datas + copy_count ;
        if(tail > 0)
        {
            size = sizeof(qkf_field_data_t) * (tail - 1) ;
            ::memcpy(from , list->datas , size) ;
        }
    }

    return true ;
}

bool qkf_field_list_copy(const qkf_field_list_t * src , qkf_field_list_t * dst)
{
    if(src == NULL || dst == NULL)
        return false ;

    qkf_field_list_final(dst) ;
    ::memset(dst , 0 , sizeof(qkf_field_list_t)) ;

    size_t bytes = src->capacity * sizeof(qkf_field_data_t) ;
    qkf_field_data_t * datas = (qkf_field_data_t *)::qkf_malloc(kDefaultMMgr , bytes) ;
    if(datas == NULL)
        return false ;
    ::memset(datas , 0 , bytes) ;
    qkf_field_list_copy_data(src , datas) ;
    //继续拷贝内容
    uint8_t type = src->type ;
    uint32_t size = src->size ;
    if(type == kTypeSTR)
    {
        for(uint32_t idx = 0 ; idx < size ; ++idx)
        {
            qkf_field_data_t * data = datas + idx ;
            if(data->str == NULL)
                continue ;
            qkf_field_str_t * str = qkf_field_str_new(data->str->data) ;
            data->str = str ;                 
        }
    }
    else if(type == kTypeRAW)
    {    
        for(uint32_t idx = 0 ; idx < size ; ++idx)
        {
            qkf_field_data_t * data = datas + idx ;
            if(data->raw == NULL)
                continue ;
            qkf_field_raw_t * raw = qkf_field_raw_new(data->raw->data , data->raw->size) ;
            data->raw = raw ;
        }
    }
    else if(type == kTypeLIST)
    {
        for(uint32_t idx = 0 ; idx < size ; ++idx)
        {
            qkf_field_data_t * data = datas + idx ;
            if(data->list == NULL)
                continue ;
            qkf_field_list_t * old_list = data->list ;
            data->list = NULL ;

            qkf_field_list_t * new_list = qkf_field_list_new(old_list->type , old_list->capacity) ;
            if(new_list == NULL)
                continue ;
            if(qkf_field_list_copy(data->list , new_list) == false)
            {
                qkf_field_list_free(new_list) ;
                continue ;
            }

            data->list = new_list ;
        }
    }
    else if(type == kTypeMAP)
    {
        for(uint32_t idx = 0 ; idx < size ; ++idx)
        {
            qkf_field_data_t * data = datas + idx ;
            if(data->map == NULL)
                continue ;
            qkf_field_map_t * old_map = data->map ;
            data->map = NULL ;

            qkf_field_map_t * new_map = qkf_field_map_new() ;
            if(new_map == NULL)
                continue ;
            if(qkf_field_map_copy(data->map , new_map) == false)
            {
                qkf_field_map_free(new_map) ;
                continue ;
            }
            data->map = new_map ;
        }
    }

    dst->type = src->type ;
    dst->capacity = src->capacity ;
    dst->size = src->size ;
    dst->tail = src->size ;
    dst->datas = datas ;

    return true ;
}

bool qkf_field_list_extend(qkf_field_list_t * list)
{
    if(list == NULL)
        return false ;

    uint32_t capacity = qkf_vector_calc_extend_size(list->capacity) ;
    size_t data_size = sizeof(qkf_field_data_t) * capacity ;
    qkf_field_data_t * datas = (qkf_field_data_t *)::qkf_malloc(kDefaultMMgr , data_size) ;
    if(datas == NULL)
        return false ;
    ::memset(datas , 0 , data_size) ;
    qkf_field_data_t * old_datas = list->datas ;

    if(qkf_field_list_copy_data(list , datas) == false)
    {
        ::qkf_free(kDefaultMMgr , datas) ;
        return false ;
    }


    list->datas = datas ;
    list->head = 0 ;
    list->tail = list->size ;
    list->capacity = capacity ;
    
    if(old_datas != NULL)
        ::qkf_free(kDefaultMMgr , old_datas) ;

    return true ;
}

bool qkf_field_list_shrink(qkf_field_list_t * list)
{
    if(list == NULL)
        return false ;
    if(list->datas == NULL || list->capacity == 0)
        return true ;

    uint32_t newcap = qkf_vector_calc_shrink_size(list->capacity , list->size) ;
    if(newcap == 0)
        return false ;

    size_t shr_size = sizeof(qkf_field_data_t) * newcap ;
    qkf_field_data_t * datas = (qkf_field_data_t *)::qkf_malloc(kDefaultMMgr , shr_size) ;
    if(datas == NULL)
        return false ;
    ::memset(datas , 0 , shr_size) ;

    qkf_field_data_t * old_datas = list->datas ;
    if(qkf_field_list_copy_data(list , datas) == false)
    {
        ::qkf_free(kDefaultMMgr , datas) ;
        return false ;
    }

    list->datas = datas ;
    list->head = 0 ;
    list->tail = list->size ;
    list->capacity = newcap ;

    if(old_datas != NULL)
        ::qkf_free(kDefaultMMgr , old_datas) ;

    return true ;
}

qkf_field_map_t * qkf_field_map_new()
{
    size_t map_size = sizeof(qkf_field_map_t) ;
    qkf_field_map_t * map = (qkf_field_map_t *)::qkf_malloc(kDefaultMMgr , map_size) ;
    if(map == NULL)
        return NULL ;
    if(qkf_field_map_init(map) == false)
    {
        ::qkf_free(kDefaultMMgr , map) ;
        return NULL ;
    }

    return map ;
}

void qkf_field_map_free(qkf_field_map_t * map)
{
    if(map == NULL)
        return ;

    qkf_field_map_final(map) ;
    ::qkf_free(kDefaultMMgr , map) ;
}

int qkf_field_map_node_compare(const rb_node_t * src , const rb_node_t * dst)
{
    if(src == NULL || dst == NULL)
        return 0 ;

    const qkf_field_map_node_t * src_node = (const qkf_field_map_node_t *)src ;
    const qkf_field_map_node_t * dst_node = (const qkf_field_map_node_t *)dst ;

    if(src_node->def.name == NULL || dst_node->def.name == NULL)
        return 0 ;

    return ::strcmp(src_node->def.name , dst_node->def.name) ;
}

bool qkf_field_map_init(qkf_field_map_t * map)
{
    if(map == NULL)
        return false ;
    map->names.root = NULL ;
    map->names.key_compare = qkf_field_map_node_compare ;

    return qkf_vector_init(&map->nodes , sizeof(qkf_field_map_node_t) , 8) ;
}

void qkf_field_map_final(qkf_field_map_t * map)
{
    if(map == NULL)
        return ;

    map->names.root = NULL ;

    //2、释放字段定义中的名字和值
    int size = qkf_vector_size(&map->nodes) ;
    for(int fidx = 0 ; fidx < size ; ++fidx)
    {
        qkf_field_map_node_t * node = (qkf_field_map_node_t *)::qkf_vector_get(&map->nodes , fidx) ;
        qkf_field_map_node_final(node) ;
    }
    qkf_vector_final(&map->nodes) ;
}

void qkf_field_map_clear(qkf_field_map_t * map) 
{
    if(map == NULL)
        return ;

    map->names.root = NULL ;

    //2、释放字段定义中的名字和值
    int size = qkf_vector_size(&map->nodes) ;
    for(int fidx = 0 ; fidx < size ; ++fidx)
    {
        qkf_field_map_node_t * node = (qkf_field_map_node_t *)::qkf_vector_get(&map->nodes , fidx) ;
        qkf_field_map_node_final(node) ;
    }
    qkf_vector_clear(&map->nodes) ;
}

void qkf_field_map_node_final(qkf_field_map_node_t * node) 
{
    if(node == NULL)
        return ;

    if(node->def.name != NULL)
    {
        ::qkf_free(kDefaultMMgr , node->def.name) ;
        node->def.name = NULL ;
    }

    uint8_t type = node->def.type ;
    if(type == kTypeSTR)
        ::qkf_free(kDefaultMMgr , node->data.str) ;
    else if(type == kTypeRAW)
        ::qkf_free(kDefaultMMgr , node->data.raw) ;
    else if(type == kTypeLIST)
        qkf_field_list_free(node->data.list) ;
    else if(type == kTypeMAP)
        qkf_field_map_free(node->data.map) ;

    node->data.val = 0 ;
}

int qkf_field_map_add(qkf_field_map_t * map , const char * name , uint8_t type , qkf_field_data_t * data)
{
    if(map == NULL || name == NULL || type < kTypeMIN || type > kTypeMAX || data == NULL)
        return -1 ;

    int index = -1 ;
    if((index = qkf_field_map_find(map , name)) < 0)
        return -1 ;

    qkf_field_map_node_t field ;
    field.def.name = qkf_strdup(name , 0) ;
    field.def.type = type ;
    field.def.index = -1 ;
    field.data.val = data->val ;

    if((index = qkf_vector_add(&map->nodes , &field)) < 0)
    {
        ::qkf_free(kDefaultMMgr , field.def.name) ;
        return -1 ;
    }
    
    qkf_field_map_node_t * node = (qkf_field_map_node_t *)qkf_vector_get(&map->nodes , index) ;
    if(node == NULL)
        return -1 ;   
    node->def.index = index ;

    if(rb_insert(&map->names , &node->link) == true)
        return index ;
    else
        return -1 ;
}

bool qkf_field_map_del(qkf_field_map_t * map , const char * name)
{
    if(map == NULL || name == NULL)
        return false ;

    int index = -1 ;
    if((index = qkf_field_map_find(map , name)) < 0)
        return false ;

    qkf_field_map_node_t * node = (qkf_field_map_node_t *)qkf_vector_get(&map->nodes , index) ;
    rb_erase(&map->names , &node->link) ;

    if(node != NULL)
        qkf_field_map_node_final(node) ;
    qkf_vector_del(&map->nodes , index) ;

    int size = qkf_vector_size(&map->nodes) ;
    for(int fidx = index ; fidx < size ; ++fidx)
    {
        qkf_field_map_node_t * node = (qkf_field_map_node_t *)qkf_vector_get(&map->nodes , fidx) ;
        node->def.index = fidx ;
    }
    return true ;
}

bool qkf_field_map_update(qkf_field_map_t * map , const char * name , qkf_field_data_t * data)
{
    if(map == NULL || name == NULL || data == NULL)
        return false ;
    int index = -1 ;
    if((index = qkf_field_map_find(map , name)) < 0)
        return false ;

    qkf_field_map_node_t * node = (qkf_field_map_node_t *)qkf_vector_get(&map->nodes , index) ;
    node->data.val = data->val ;
    return true ;
}

int qkf_field_map_size(qkf_field_map_t * map)
{
    return (map == NULL) ? 0 : qkf_vector_size(&map->nodes) ;
}

bool qkf_field_map_get(qkf_field_map_t * map , int index , qkf_field_map_node_t*&node)
{
    if(map == NULL)
        return false ;

    node = (qkf_field_map_node_t *)::qkf_vector_get(&map->nodes , index) ;
    return (node != NULL) ;
}

int qkf_field_map_find(qkf_field_map_t * map , const char * name)
{
    if(map == NULL || name == NULL)
        return -1 ;

    qkf_field_map_node_t node ;
    node.def.name = (char *)name ;

    qkf_field_map_node_t * f = (qkf_field_map_node_t *)rb_find(&map->names , &node.link) ;
    if(f == NULL)
        return -1 ;

    return f->def.index ;
}

bool qkf_field_map_copy(const qkf_field_map_t * src , qkf_field_map_t * dst) 
{
    if(src == NULL || dst == NULL)
        return false ;
    qkf_field_map_final(dst) ;

    dst->names.root = NULL ;
    dst->names.key_compare = qkf_field_map_node_compare ;
    if(qkf_vector_init(&dst->nodes , sizeof(qkf_field_map_node_t) , src->nodes.capacity) == false)
        return false ;

    int size = ::qkf_vector_size(&src->nodes) ;
    for(int idx = 0 ; idx < size ; ++idx)
    {
        qkf_field_map_node_t * snode = (qkf_field_map_node_t *)::qkf_vector_get((qkf_vector_t *)&src->nodes , idx) ;
        qkf_field_map_node_t * dnode = (qkf_field_map_node_t *)::qkf_vector_get((qkf_vector_t *)&dst->nodes , idx) ;

        dnode->def.type = snode->def.type ;
        dnode->def.index = idx ;
        dnode->def.offset = sizeof(qkf_field_data_t) * idx ;
        if(snode->def.name != NULL)
            dnode->def.name = ::qkf_strdup(snode->def.name , 0) ;

        dnode->data.val = 0 ;
        qkf_field_data_t * sdata = &snode->data ;
        uint8_t type = snode->def.type ;
        if(type == kTypeSTR)
            dnode->data.str = ::qkf_field_str_new(sdata->str->data) ;
        else if(type == kTypeRAW)
            dnode->data.raw = ::qkf_field_raw_new(sdata->raw->data , sdata->raw->size) ;
        else if(type == kTypeLIST)
        {
            qkf_field_list_t * src_list = sdata->list ;
            if(src_list != NULL)
            {
                dnode->data.list = ::qkf_field_list_new(src_list->type , src_list->capacity) ;
                if(dnode->data.list != NULL)
                    qkf_field_list_copy(src_list , dnode->data.list) ;
            }
        }

        rb_insert(&dst->names , &dnode->link) ;
    }

    return true ;
}
