
#include "config.h"

#include <glib.h>
#include <glib/gprintf.h>
#include <glib/gi18n.h>
#include <locale.h>
#include <gtk/gtk.h>

#include <stdio.h>
#include <sys/stat.h>

#include "defs.h"
#include "sylmain.h"
#include "mainwindow.h"
#include "plugin.h"
#include "procmsg.h"
#include "procmime.h"
#include "procheader.h"
#include "utils.h"
#include "alertpanel.h"
#include "prefs_common.h"
#include "foldersel.h"
#include "account.h"

#include "copying.h"
#include "sylplugin_factory.h"
#include "switch_signatures.h"

static SylPluginInfo info = {
  N_(PLUGIN_NAME),
  VERSION,
  "HAYASHI Kentaro",
  N_(PLUGIN_DESC)
};

SwitchSignaturesOption SYLPF_OPTION;

SwitchSignature current_signature;

void plugin_load(void)
{
  void *option;

  SYLPF_START_FUNC;

  sylpf_init_gettext(SWITCH_SIGNATURES, "lib/locale");

  syl_plugin_signal_connect("compose-created",
                            G_CALLBACK(compose_created_cb), NULL);

  syl_plugin_add_menuitem("/Tools", NULL, NULL, NULL);
  syl_plugin_add_menuitem("/Tools",
                          N_("Switch signature settings [switch_signatures]"),
                          preference_menu_cb,
                          NULL);

  info.name = g_strdup(_(PLUGIN_NAME));
  info.description = g_strdup(_(PLUGIN_DESC));

  option = &SYLPF_OPTION;
  sylpf_load_option_rcfile(option, SYLPF_PLUGIN_RC);

  SYLPF_END_FUNC;
}

void plugin_unload(void)
{
  g_free(SYLPF_OPTION.rcpath);
}

SylPluginInfo *plugin_info(void)
{
  return &info;
}

gint plugin_interface_version(void)
{
  /* sylpheed 3.2 or later since r3005 */
  return 0x0109;
}

static void compose_created_cb(GObject *obj, gpointer data)
{
  Compose *compose;
  guint signal_id;
  guint n_matched;
  GList *list, *account_list;
  gint index;
  PrefsAccount *account;
  SwitchSignaturePair *pair;

  SYLPF_START_FUNC;

  compose = data;
  
  signal_id = g_signal_lookup("button_press_event", GTK_TYPE_BUTTON);
  SYLPF_DEBUG_VAL("signal_id", signal_id);
  n_matched = g_signal_handlers_disconnect_matched(G_OBJECT(compose->sig_btn),
                                                   G_SIGNAL_MATCH_ID | G_SIGNAL_MATCH_DATA,
                                                   signal_id, 0,
                                                   NULL, NULL, compose);
  SYLPF_DEBUG_VAL("removed signal", n_matched);

  signal_id = g_signal_lookup("clicked", GTK_TYPE_TOOL_BUTTON);
  SYLPF_DEBUG_VAL("signal_id", signal_id);
  n_matched = g_signal_handlers_disconnect_matched(G_OBJECT(compose->sig_btn),
                                                   G_SIGNAL_MATCH_ID | G_SIGNAL_MATCH_DATA,
                                                   signal_id, 0,
                                                   NULL, NULL, compose);
  SYLPF_DEBUG_VAL("removed signal", n_matched);
  
  current_signature.compose = compose;
  current_signature.signature_index = 0;

  account_list = account_get_list();

  list = NULL;
  for (index = 0; index < g_list_length(account_list); index++) {
    account = g_list_nth_data(account_list, index);
    if (account->sig_text) {
      list = g_list_append(list, account->sig_text);
    }
  }
  account_list = get_signatures_list();
  for (index = 0; index < g_list_length(account_list); index++) {
    pair = g_list_nth_data(account_list, index);
    if (pair) {
      if (pair->signature) {
        list = g_list_append(list, pair->signature);
      }
    }
  }
  current_signature.signatures = list;
  g_signal_connect(GTK_WIDGET(compose->sig_btn), "clicked",
                   G_CALLBACK(switch_signature_cb),
                   &current_signature);

  SYLPF_END_FUNC;
}

static void switch_signature_cb(GtkWidget *widget, gpointer data)
{
  SwitchSignature *signs;
  gchar *signature;
  gboolean sign_found;
  GtkTextIter iter;
  GtkTextIter start_iter;
  GtkTextIter end_iter;
  GtkTextBuffer *buffer;
  PrefsCommon *prefs;
  guint index;

  SYLPF_START_FUNC;

  signs = data;
  
  g_return_if_fail(signs != NULL);
  g_return_if_fail(signs->signature_index >= 0);
  g_return_if_fail(signs->signatures != NULL);
  g_return_if_fail(signs->compose != NULL);

  if (signs->signature_index >= g_list_length(signs->signatures)) {
    signs->signature_index = 0;
  }

  for (index = 0; index < g_list_length(signs->signatures); index++) {
    signature = g_list_nth_data(signs->signatures, index);
    SYLPF_DEBUG_VAL("signature index", index);
    SYLPF_DEBUG_STR("signature text", signature);
  }

  signature = g_list_nth_data(signs->signatures, signs->signature_index);
    
  buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(signs->compose->text));

  SYLPF_DEBUG_PTR("buffer", buffer);

  gtk_text_buffer_get_bounds(buffer,
                             &start_iter,
                             &end_iter);

  SYLPF_DEBUG_PTR("start_iter", &start_iter);
  SYLPF_DEBUG_PTR("end_iter", &end_iter);

  SYLPF_DEBUG_STR("range text",
                  gtk_text_buffer_get_text(buffer, &start_iter, &end_iter, TRUE));
    

  prefs = prefs_common_get();

  sign_found = gtk_text_iter_backward_search(&end_iter,
                                             prefs->sig_sep,
                                             0,
                                             &iter,
                                             NULL,
                                             NULL);
  if (sign_found) {
    SYLPF_DEBUG_MSG("signature marker found");
    gtk_text_buffer_delete(buffer, &iter, &end_iter);
    signature = g_strdup_printf("%s\n%s",
                                prefs->sig_sep,
                                g_strcompress(signature));
    gtk_text_buffer_insert(buffer, &end_iter,
                           signature,
                           g_utf8_strlen(signature, -1));
    signs->signature_index++;
  }

  SYLPF_END_FUNC;
}

static void save_preference(SwitchSignaturesOption *option)
{
  SYLPF_START_FUNC;

  SYLPF_END_FUNC;
}

static void preference_menu_cb(void)
{
  GtkWidget *dialog;
  gint response;

  SYLPF_START_FUNC;

  dialog = create_preference_dialog(&SYLPF_OPTION);

  gtk_widget_show_all(dialog);
  response = gtk_dialog_run(GTK_DIALOG(dialog));

  switch (response) {
  case GTK_RESPONSE_OK:
    save_preference(&SYLPF_OPTION);
    break;
  case GTK_RESPONSE_CANCEL:
  default:
    break;
  }

  gtk_widget_destroy(dialog);

  SYLPF_END_FUNC;
}

static GtkWidget *create_preference_dialog(SwitchSignaturesOption *option)
{
  GtkWidget *vbox, *hbox;
  GtkWidget *notebook;
  GtkWidget *dialog;
  GtkWidget *window;
  GtkWidget *page;
  GtkWidget *label;
  gpointer mainwin;

  SYLPF_START_FUNC;

  mainwin = syl_plugin_main_window_get();
  window = ((MainWindow*)mainwin)->window;

  dialog = gtk_dialog_new_with_buttons(_("Switch Signatures"),
                                       GTK_WINDOW(window),
                                       GTK_DIALOG_MODAL,
                                       GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL,
                                       GTK_STOCK_OK, GTK_RESPONSE_OK,
                                       NULL);

  sylpf_init_preference_dialog_size(dialog);

  vbox = gtk_vbox_new(FALSE, SYLPF_BOX_SPACE);
  hbox = gtk_hbox_new(TRUE, SYLPF_BOX_SPACE);

  gtk_container_add(GTK_CONTAINER(hbox), vbox);
  gtk_container_add(GTK_CONTAINER(GTK_DIALOG(dialog)->vbox), hbox);

  notebook = gtk_notebook_new();
  page = create_config_main_page(notebook, SYLPF_OPTION.rcfile);
  label = gtk_label_new(_("General"));
  gtk_notebook_append_page(GTK_NOTEBOOK(notebook), page, label);

  page = create_config_about_page(notebook, SYLPF_OPTION.rcfile);
  label = gtk_label_new(_("About"));
  gtk_notebook_append_page(GTK_NOTEBOOK(notebook), page, label);

  gtk_box_pack_start(GTK_BOX(vbox), notebook, TRUE, TRUE, 0);

  gtk_box_set_spacing(GTK_BOX(GTK_DIALOG(dialog)->vbox), SYLPF_BOX_SPACE);

  SYLPF_RETURN_VALUE(dialog);
}

static GtkWidget *create_config_main_page(GtkWidget *notebook, GKeyFile *pkey)
{
  GtkWidget *vbox;
  GtkWidget *page;
  GtkWidget *edit_frame;
  GtkWidget *manage_frame;
  GtkWidget *area;
  GtkWidget *buttons;

  SYLPF_START_FUNC;

  page = gtk_vbox_new(FALSE, SYLPF_BOX_SPACE);
  vbox = gtk_vbox_new(FALSE, SYLPF_BOX_SPACE);

  area = create_signatures_store();
  buttons = create_signatures_manage_buttons();

  gtk_box_pack_start(GTK_BOX(vbox), area, FALSE, FALSE, 0);
  gtk_box_pack_start(GTK_BOX(vbox), buttons, FALSE, FALSE, 0);

  manage_frame = sylpf_pack_widget_with_aligned_frame(vbox,
                                                      _("Manage signatures"));
  gtk_box_pack_start(GTK_BOX(page), manage_frame, FALSE, FALSE, 0);


  vbox = gtk_vbox_new(FALSE, SYLPF_BOX_SPACE);

  area = create_signatures_edit_area();
  buttons = create_signatures_edit_buttons();

  gtk_box_pack_start(GTK_BOX(vbox), area, TRUE, TRUE, 0);
  gtk_box_pack_start(GTK_BOX(vbox), buttons, FALSE, FALSE, 0);

  edit_frame = sylpf_pack_widget_with_aligned_frame(vbox, _("Edit signatures"));
  gtk_box_pack_start(GTK_BOX(page), edit_frame, TRUE, TRUE, 0);


  SYLPF_RETURN_VALUE(page);
}

static GtkWidget *create_signatures_manage_buttons(void)
{
  GtkWidget *hbox;
  GtkWidget *new_signature;
  GtkWidget *edit_signature;
  GtkWidget *delete_signature;

  hbox = gtk_hbox_new(FALSE, 0);
  new_signature = gtk_button_new_from_stock(GTK_STOCK_NEW);
  edit_signature = gtk_button_new_from_stock(GTK_STOCK_EDIT);
  delete_signature = gtk_button_new_from_stock(GTK_STOCK_DELETE);
  gtk_box_pack_end(GTK_BOX(hbox), delete_signature, FALSE, FALSE, 0);
  gtk_box_pack_end(GTK_BOX(hbox), edit_signature, FALSE, FALSE, 0);
  gtk_box_pack_end(GTK_BOX(hbox), new_signature, FALSE, FALSE, 0);

  g_signal_connect(GTK_WIDGET(delete_signature), "clicked",
                   G_CALLBACK(delete_current_signature_cb),
                   &current_signature);
  g_signal_connect(GTK_WIDGET(edit_signature), "clicked",
                   G_CALLBACK(edit_current_signature_cb),
                   &current_signature);
  g_signal_connect(GTK_WIDGET(new_signature), "clicked",
                   G_CALLBACK(new_current_signature_cb),
                   &current_signature);

  /* FIXME: */
  gtk_widget_set_sensitive(delete_signature, FALSE);
  gtk_widget_set_sensitive(edit_signature, FALSE);
  
  return hbox;
}



static GList *get_signatures_list(void)
{
  gint n_signatures;
  gint signature_no;
  gchar *key;
  gchar *label;
  gchar *signature;
  gchar *path;
  gsize n_length;
  GError *error;
  SwitchSignaturePair *pair;
  GList *list;
  gboolean succeeded;
  
  SYLPF_START_FUNC;

  n_signatures = SYLPF_GET_RC_INTEGER(SYLPF_OPTION.rcfile,
                                      SYLPF_ID, "signatures");
  SYLPF_DEBUG_VAL("n_signatures", n_signatures);

  list = NULL;

  if (n_signatures > 0) {
    for (signature_no = 1; signature_no < n_signatures; signature_no++) {
      pair = g_new(SwitchSignaturePair, 1);

      key = g_strdup_printf("signatures%d", signature_no);
      label = SYLPF_GET_RC_STRING(SYLPF_OPTION.rcfile, SYLPF_ID, key);
      pair->key = key;
      pair->label = label;

      SYLPF_DEBUG_STR("label", label);

      key = g_strdup_printf("%d.txt", signature_no);
      path = g_build_path(G_DIR_SEPARATOR_S,
                          get_rc_dir(),
                          "plugins",
                          SYLPF_ID,
                          key,
                          NULL);
      pair->path = path;
      SYLPF_DEBUG_STR("signature file", path);

      succeeded = g_file_get_contents(path, &signature, &n_length, &error);
      pair->signature = signature;

      SYLPF_DEBUG_STR("signature", signature);

      list = g_list_append(list, pair);
      if (!succeeded) {
        g_error_free(error);
      }
    }
  }
  SYLPF_RETURN_VALUE(list);
}

static GtkWidget *create_signatures_store(void)
{
  GtkWidget *hbox;
  GtkWidget *tree;
  GtkTreeIter iter;
  GtkTreeStore *store;
  GtkTreeViewColumn *column;
  GtkCellRenderer *renderer;
  gint n_signatures;
  GList *account_list;
  gint n_accounts;
  gint index;
  PrefsAccount *account;
  SwitchSignaturePair *pair;

  SYLPF_START_FUNC;

  hbox = gtk_hbox_new(FALSE, 0);

  store = gtk_tree_store_new(N_SIGNATURE_COLUMNS,
                             G_TYPE_STRING,
                             G_TYPE_STRING,
                             G_TYPE_STRING);

  account_list = account_get_list();
  n_accounts = g_list_length(account_list);

  for (index = 0; index < n_accounts; index++) {
    account = g_list_nth_data(account_list, index);
    if (account->sig_text) {
      gtk_tree_store_append(store, &iter, NULL);
      gtk_tree_store_set(store, &iter,
                         SIGNATURE_ACCOUNT_COLUMN,
                         account->account_name ? account->account_name : "",
                         SIGNATURE_SUMMARY_COLUMN, account->sig_text,
                         -1);
    }
  }

  account_list = get_signatures_list();
  n_signatures = g_list_length(account_list);
  for (index = 0; index < n_signatures; index++) {
    pair = g_list_nth_data(account_list, index);

    gtk_tree_store_append(store, &iter, NULL);
    gtk_tree_store_set(store, &iter,
                       SIGNATURE_ACCOUNT_COLUMN, pair->label,
                       SIGNATURE_SUMMARY_COLUMN, pair->signature,
                       -1);
  }

  tree = gtk_tree_view_new_with_model(GTK_TREE_MODEL(store));

  renderer = gtk_cell_renderer_text_new();
  column = gtk_tree_view_column_new_with_attributes(_("Label"),
                                                    renderer,
                                                    "text",
                                                    SIGNATURE_ACCOUNT_COLUMN,
                                                    NULL);
  gtk_tree_view_append_column(GTK_TREE_VIEW(tree), column);

  column = gtk_tree_view_column_new_with_attributes(_("Signature"),
                                                    renderer,
                                                    "text",
                                                    SIGNATURE_SUMMARY_COLUMN,
                                                    NULL);
  gtk_tree_view_append_column(GTK_TREE_VIEW(tree), column);

  gtk_box_pack_start(GTK_BOX(hbox), tree, TRUE, TRUE, 0);

  current_signature.store = store;
  current_signature.tree = tree;

  SYLPF_RETURN_VALUE(hbox);
}

static GtkWidget *create_signatures_edit_area(void)
{
  GtkWidget *hbox, *vbox;
  GtkWidget *scrolled;
  GtkTextBuffer *tbuffer;
  GtkWidget *tview;

  SYLPF_START_FUNC;

  hbox = gtk_hbox_new(TRUE, SYLPF_BOX_SPACE);
  vbox = gtk_vbox_new(TRUE, SYLPF_BOX_SPACE);
  gtk_box_pack_start(GTK_BOX(hbox), vbox, TRUE, TRUE, SYLPF_BOX_SPACE);

  scrolled = gtk_scrolled_window_new(NULL, NULL);

  tbuffer = gtk_text_buffer_new(NULL);
  tview = gtk_text_view_new_with_buffer(tbuffer);
  gtk_text_view_set_editable(GTK_TEXT_VIEW(tview), TRUE);
  gtk_container_add(GTK_CONTAINER(scrolled), tview);

  gtk_box_pack_start(GTK_BOX(vbox), scrolled, TRUE, TRUE, SYLPF_BOX_SPACE);

  SYLPF_RETURN_VALUE(hbox);
}

static GtkWidget *create_signatures_edit_buttons(void)
{
  GtkWidget *hbox;
  GtkWidget *add_signature;
  GtkWidget *new_signature;

  hbox = gtk_hbox_new(FALSE, 0);
  add_signature = gtk_button_new_from_stock(GTK_STOCK_SAVE);
  new_signature = gtk_button_new_from_stock(GTK_STOCK_NEW);
  gtk_box_pack_end(GTK_BOX(hbox), add_signature, FALSE, FALSE, 0);
  gtk_box_pack_end(GTK_BOX(hbox), new_signature, FALSE, FALSE, 0);

  g_signal_connect(GTK_WIDGET(add_signature), "clicked",
                   G_CALLBACK(add_current_signature_cb),
                   &current_signature);
  g_signal_connect(GTK_WIDGET(new_signature), "clicked",
                   G_CALLBACK(new_current_signature_cb),
                   &current_signature);

  /* FIXME: */
  gtk_widget_set_sensitive(add_signature, FALSE);
  gtk_widget_set_sensitive(new_signature, FALSE);

  return hbox;
}

static void edit_current_signature_cb(GtkWidget *widget,
                                      gpointer data)
{
  syl_plugin_alertpanel_message("", "Not Implemented yet", ALERT_NOTICE);
}

static void delete_current_signature_cb(GtkWidget *widget,
                                        gpointer data)
{
  syl_plugin_alertpanel_message("", "Not Implemented yet", ALERT_NOTICE);
}

static void add_current_signature_cb(GtkWidget *widget,
                                     gpointer data)
{
  syl_plugin_alertpanel_message("", "Not Implemented yet", ALERT_NOTICE);
}

static void new_current_signature_cb(GtkWidget *widget,
                                     gpointer data)
{
  syl_plugin_alertpanel_message("", "Not Implemented yet", ALERT_NOTICE);
}

static GtkWidget *create_config_about_page(GtkWidget *notebook, GKeyFile *pkey)
{

  GtkWidget *hbox;
  GtkWidget *vbox;
  GtkWidget *misc;
  GtkWidget *scrolled;
  GtkTextBuffer *tbuffer;
  GtkWidget *tview;

  SYLPF_START_FUNC;

  hbox = gtk_hbox_new(TRUE, SYLPF_BOX_SPACE);
  vbox = gtk_vbox_new(FALSE, SYLPF_BOX_SPACE);
  gtk_box_pack_start(GTK_BOX(hbox), vbox, TRUE, TRUE, SYLPF_BOX_SPACE);

  misc = gtk_label_new(_("SwitchSignatures"));
  gtk_box_pack_start(GTK_BOX(vbox), misc, FALSE, TRUE, SYLPF_BOX_SPACE);

  misc = gtk_label_new(_(PLUGIN_DESC));
  gtk_box_pack_start(GTK_BOX(vbox), misc, FALSE, TRUE, SYLPF_BOX_SPACE);

  scrolled = gtk_scrolled_window_new(NULL, NULL);

  tbuffer = gtk_text_buffer_new(NULL);
  gtk_text_buffer_set_text(tbuffer, _(copyright),
                           strlen(copyright));
  tview = gtk_text_view_new_with_buffer(tbuffer);
  gtk_text_view_set_editable(GTK_TEXT_VIEW(tview), FALSE);
  gtk_container_add(GTK_CONTAINER(scrolled), tview);

  gtk_box_pack_start(GTK_BOX(vbox), scrolled, TRUE, TRUE, 6);

  SYLPF_RETURN_VALUE(hbox);
}
