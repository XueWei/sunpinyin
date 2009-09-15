#include <cassert>
#include "sunpinyin_engine.h"
#include "sunpinyin_config_keys.h"
#include "sunpinyin_config.h"

struct ConfigItem
{
    std::string section;
    std::string name;
    ConfigItem(const std::string& key)
    {
        size_t pos = key.rfind('/');
        assert (pos != key.npos);
        section = key.substr(0, pos);
        name = key.substr(pos);
    }
};

SunPinyinConfig::SunPinyinConfig()
{
    m_scheme_names["QuanPin"]    = CSunpinyinSessionFactory::QUANPIN;
    m_scheme_names["ShuangPin"]  = CSunpinyinSessionFactory::SHUANGPIN;
    m_type_names["MS2003"]       = MS2003;
    m_type_names["ABC"]          = ABC;
    m_type_names["ZIRANMA"]      = ZIRANMA;
    m_type_names["PINYINJIAJIA"] = PINYINJIAJIA;
    m_type_names["ZIGUANG"]      = ZIGUANG;
    m_type_names["USERDEFINE"]   = USERDEFINE;
}

IBusConfig *
SunPinyinConfig::m_config = NULL;

bool
SunPinyinConfig::get(const char* key, bool val)
{
    assert(m_config != NULL);
    
    GValue v = {0};
    gboolean got;
    ConfigItem item(key);
    got = ibus_config_get_value(m_config, item.section.c_str(), item.name.c_str(), &v);
    bool result = val;
    if (got) {
        result = (g_value_get_boolean(&v) == TRUE);
    }
    return result;
}

void
SunPinyinConfig::set(const char* key, bool value)
{
    assert(m_config != NULL);
    
    GValue v = {0};
    g_value_init(&v, G_TYPE_BOOLEAN);
    g_value_set_boolean(&v, value?TRUE:FALSE);
    ConfigItem item(key);
    ibus_config_set_value(m_config, item.section.c_str(), item.name.c_str(), &v);
}

std::string
SunPinyinConfig::get(const char* key, const std::string& val)
{
    assert(m_config != NULL);
    
    GValue v = {0};
    gboolean got;
    ConfigItem item(key);
    got = ibus_config_get_value(m_config, item.section.c_str(), item.name.c_str(), &v);
    std::string result = val;
    if (got) {
        result = std::string(g_value_get_string(&v));
    }
    return result;
}

void
SunPinyinConfig::set(const char* key, const std::string& val)
{
    assert(m_config != NULL);
    
    GValue v = {0};
    g_value_init(&v, G_TYPE_STRING);
    g_value_set_string(&v, val.c_str());
    ConfigItem item(key);
    ibus_config_set_value(m_config, item.section.c_str(), item.name.c_str(), &v);
}

unsigned
SunPinyinConfig::get(const char* key, unsigned val)
{
    assert(m_config != NULL);
    
    GValue v = {0};
    gboolean got;
    ConfigItem item(key);
    got = ibus_config_get_value(m_config, item.section.c_str(), item.name.c_str(), &v);
    unsigned result = val;
    if (got) {
        result =  g_value_get_uint(&v);
    }
    return result;
}

void 
SunPinyinConfig::set(const char* key, unsigned value)
{
    assert(m_config != NULL);
    
    GValue v = {0};
    g_value_init(&v, G_TYPE_UINT);
    g_value_set_uint(&v, value);
    ConfigItem item(key);
    ibus_config_set_value(m_config, item.section.c_str(), item.name.c_str(), &v);
}


CSunpinyinSessionFactory::EPyScheme
SunPinyinConfig::get_py_scheme(CSunpinyinSessionFactory::EPyScheme scheme)
{
    std::string default_name =
        get_scheme_name(scheme);
    std::string name = get(CONFIG_PINYIN_SCHEME, default_name);
    return get_scheme(name);
}

void
SunPinyinConfig::set_py_scheme(CSunpinyinSessionFactory::EPyScheme scheme)
{
    std::string name =
        get_scheme_name(scheme);
    set(CONFIG_PINYIN_SCHEME, name);
}

EShuangpinType
SunPinyinConfig::get_shuangpin_type(EShuangpinType type)
{
    std::string default_name = get_type_name(type);
    std::string name = get(CONFIG_PINYIN_SHUANGPIN_TYPE, default_name);
    return get_type(name);
}

void
SunPinyinConfig::set_shuangpin_type(EShuangpinType type)
{
    std::string name = get_type_name(type);
    set(CONFIG_PINYIN_SHUANGPIN_TYPE, name);
}

void
SunPinyinConfig::set_config(IBusConfig *config)
{
    m_config = config;
}

void
SunPinyinConfig::listen_on_changed(SunPinyinEngine *engine)
{
    assert(m_config != NULL);
    g_signal_connect(m_config, "value-changed",
                     G_CALLBACK(this->on_config_value_changed), this);
    m_engine = engine;
}

void
SunPinyinConfig::on_config_value_changed(IBusConfig *config,
                                         const gchar *section,
                                         const gchar *name,
                                         GValue *value,
                                         gpointer user_data)
{
    SunPinyinConfig *thiz = reinterpret_cast<SunPinyinConfig*>(user_data);
    thiz->m_engine->update_config();
}

std::string
SunPinyinConfig::get_scheme_name(CSunpinyinSessionFactory::EPyScheme scheme)
{
    std::string val = "ShuangPin";
    for (SchemeNames::iterator it = m_scheme_names.begin();
         it != m_scheme_names.end(); ++it) {
        if (it->second == scheme)
            val = it->first;
    }
    return val;
}

CSunpinyinSessionFactory::EPyScheme
SunPinyinConfig::get_scheme(const std::string& name)
{
    CSunpinyinSessionFactory::EPyScheme val = CSunpinyinSessionFactory::SHUANGPIN;
    SchemeNames::iterator it = m_scheme_names.find(name);
    if (it != m_scheme_names.end()) {
        val = it->second;
    }
    return val;
}


std::string
SunPinyinConfig::get_type_name(EShuangpinType type)
{
    std::string val = "MS2003";
    for (TypeNames::iterator it = m_type_names.begin();
         it != m_type_names.end(); ++it) {
        if (it->second == type)
            val = it->first;
    }
    return val;
}

EShuangpinType
SunPinyinConfig::get_type(const std::string& name)
{
    EShuangpinType val = MS2003;
    TypeNames::iterator it = m_type_names.find(name);
    if (it != m_type_names.end()) {
        val = it->second;
    }
    return val;
}