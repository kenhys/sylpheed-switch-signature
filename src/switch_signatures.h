#ifndef __SYLPF_SWITCH_SIGNATURES_H_INCLUDED__
#define __SYLPF_SWITCH_SIGNATURES_H_INCLUDED__

#undef SYLPF_PLUGIN_RC
#define SYLPF_PLUGIN_RC "switchsignaturesrc"

#define SWITCH_SIGNATURES "switch_signatures"

#ifdef PLUGIN_NAME
#undef PLUGIN_NAME
#endif
#define PLUGIN_NAME "SwitchSignatures"

#ifdef PLUGIN_DESC
#undef PLUGIN_DESC
#endif
#define PLUGIN_DESC "Switch multiple signatures for Sylpheed"

struct _SwitchSignaturesOption {
  gchar *rcpath;
  GKeyFile *rcfile;

  gboolean plugin_enabled;

  GtkWidget *plugin_on;
  GtkWidget *plugin_off;
  GtkWidget *plugin_switch;
  GtkTooltips *plugin_tooltip;
};
typedef struct  _SwitchSignaturesOption SwitchSignaturesOption;

struct _SwitchSignature
{
  gchar *account;
  gchar *mail;
  gchar *signature;

  GtkTreeStore *store;
  GtkWidget *tree;

  Compose *compose;

  gint signature_index;
  GList *signatures;
};
typedef struct _SwitchSignature SwitchSignature;

enum {
  SIGNATURE_ACCOUNT_COLUMN,
  SIGNATURE_SUMMARY_COLUMN,
  N_SIGNATURE_COLUMNS
};

#undef SYLPF_OPTION_P
#define SYLPF_OPTION_P (SwitchSignaturesOption*)&SYLPF_OPTION

static void compose_created_cb(GObject *obj, gpointer compose);
static void preference_menu_cb(void);
static GtkWidget *create_preference_dialog(SwitchSignaturesOption *option);

static GtkWidget *create_config_main_page(GtkWidget *notebook, GKeyFile *pkey);
static GtkWidget *create_config_about_page(GtkWidget *notebook, GKeyFile *pkey);
static GtkWidget *create_signatures_store(void);
static GtkWidget *create_signatures_manage_buttons(void);

static GtkWidget *create_signatures_edit_area(void);
static GtkWidget *create_signatures_edit_buttons(void);

static void edit_current_signature_cb(GtkWidget *widget,
                                      gpointer data);
static void delete_current_signature_cb(GtkWidget *widget,
                                        gpointer data);
static void add_current_signature_cb(GtkWidget *widget,
                                     gpointer data);
static void new_current_signature_cb(GtkWidget *widget,
                                     gpointer data);

static void switch_signature_cb(GtkWidget *widget,
                                gpointer data);

#endif