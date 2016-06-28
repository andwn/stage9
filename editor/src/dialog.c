#include "dialog.h"
#include <gtk/gtk.h>
#include <stdlib.h>
#include <string.h>
#include "log.h"

GtkFileFilter* pattern_filter(const char *name, const char *pattern) {
	GtkFileFilter *filter = gtk_file_filter_new();
	gtk_file_filter_set_name(filter, name);
	gtk_file_filter_add_pattern(filter, pattern);
	return filter;
}

MapDialogResult* dialog_map_edit(const Map *map) {
	MapDialogResult *result = calloc(sizeof(MapDialogResult), 1);
	result->cancelled = true;
	GtkDialog *dialog = GTK_DIALOG(gtk_dialog_new_with_buttons(
		map == NULL ? "New Map" : "Map Properties", NULL, 0, 
		GTK_STOCK_OK, GTK_RESPONSE_ACCEPT, GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL, NULL));
	// Name of map
	GtkWidget *lblName = gtk_label_new("Name");
	GtkEntry *txtName = GTK_ENTRY(gtk_entry_new_with_max_length(31));
	GtkBox *boxName = GTK_BOX(gtk_hbox_new(false, 8));
	gtk_box_pack_start_defaults(boxName, lblName);
	gtk_box_pack_start_defaults(boxName, GTK_WIDGET(txtName));
	// Width & height spinner
	GtkWidget *lblWidth = gtk_label_new("Width");
	GtkWidget *lblHeight = gtk_label_new("Height");
	GtkSpinButton *numWidth = GTK_SPIN_BUTTON(gtk_spin_button_new_with_range(20, 999, 1));
	GtkSpinButton *numHeight = GTK_SPIN_BUTTON(gtk_spin_button_new_with_range(14, 999, 1));
	GtkBox *boxWidth = GTK_BOX(gtk_hbox_new(false, 8));
	GtkBox *boxHeight = GTK_BOX(gtk_hbox_new(false, 8));
	gtk_box_pack_start_defaults(boxWidth, lblWidth);
	gtk_box_pack_start_defaults(boxWidth, GTK_WIDGET(numWidth));
	gtk_box_pack_start_defaults(boxHeight, lblHeight);
	gtk_box_pack_start_defaults(boxHeight, GTK_WIDGET(numHeight));
	// Check boxes for flags
	GtkWidget *lblOptions = gtk_label_new("Options");
	GtkToggleButton *chkOptions[5] = {
		GTK_TOGGLE_BUTTON(gtk_check_button_new_with_label("Enable Upper Layer")),
		GTK_TOGGLE_BUTTON(gtk_check_button_new_with_label("Draw map to PLAN_A")),
		GTK_TOGGLE_BUTTON(gtk_check_button_new_with_label("Single byte tiles")),
		GTK_TOGGLE_BUTTON(gtk_check_button_new_with_label("Wrap Horizontal")),
		GTK_TOGGLE_BUTTON(gtk_check_button_new_with_label("Wrap Vertical"))
	};
	// Stack these into the dialog vbox
	gtk_box_pack_start_defaults(GTK_BOX(dialog->vbox), GTK_WIDGET(boxName));
	gtk_box_pack_start_defaults(GTK_BOX(dialog->vbox), GTK_WIDGET(boxWidth));
	gtk_box_pack_start_defaults(GTK_BOX(dialog->vbox), GTK_WIDGET(boxHeight));
	gtk_box_pack_start_defaults(GTK_BOX(dialog->vbox), lblOptions);
	for(int i = 0; i < 5; i++)
		gtk_box_pack_start_defaults(GTK_BOX(dialog->vbox), GTK_WIDGET(chkOptions[i]));
	// Set current or default values
	if(map == NULL) { // New Map
		gtk_entry_set_text(txtName, "Untitled");
		gtk_spin_button_set_value(numWidth, 40);
		gtk_spin_button_set_value(numHeight, 28);
		gtk_toggle_button_set_active(chkOptions[0], true);
		gtk_toggle_button_set_active(chkOptions[1], false);
		gtk_toggle_button_set_active(chkOptions[2], true);
		gtk_toggle_button_set_active(chkOptions[3], false);
		gtk_toggle_button_set_active(chkOptions[4], false);
	} else { // Edit Existing Map
		if(map->name != NULL) gtk_entry_set_text(txtName, map->name);
		gtk_spin_button_set_value(numWidth, map->width);
		gtk_spin_button_set_value(numHeight, map->height);
		gtk_toggle_button_set_active(chkOptions[0], map->upperLayer);
		gtk_toggle_button_set_active(chkOptions[1], map->planA);
		gtk_toggle_button_set_active(chkOptions[2], map->byteTiles);
		gtk_toggle_button_set_active(chkOptions[3], map->wrapH);
		gtk_toggle_button_set_active(chkOptions[4], map->wrapV);
	}
	// Finally run the dialog
	gtk_widget_show_all(GTK_WIDGET(dialog));
	if(gtk_dialog_run(dialog) == GTK_RESPONSE_ACCEPT) {
		result->cancelled = false;
		// Grab the new name
		const char *mn = gtk_entry_get_text(txtName);
		result->name = malloc(strlen(mn) + 1);
		strcpy(result->name, mn);
		// Width, height
		result->width = gtk_spin_button_get_value_as_int(numWidth);
		result->height = gtk_spin_button_get_value_as_int(numHeight);
		// Options
		result->upperLayer = gtk_toggle_button_get_active(chkOptions[0]);
		result->planA = gtk_toggle_button_get_active(chkOptions[1]);
		result->byteTiles = gtk_toggle_button_get_active(chkOptions[2]);
		result->wrapH = gtk_toggle_button_get_active(chkOptions[3]);
		result->wrapV = gtk_toggle_button_get_active(chkOptions[4]);
	}
	gtk_widget_destroy(GTK_WIDGET(dialog));
	while(gtk_events_pending()) gtk_main_iteration();
	return result;
}

char* dialog_map_open() {
	char *filename = NULL;
	GtkWidget *dialog = gtk_file_chooser_dialog_new("Open Map", NULL,
		GTK_FILE_CHOOSER_ACTION_OPEN, GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL,
		GTK_STOCK_OPEN, GTK_RESPONSE_ACCEPT, NULL);
	gtk_file_chooser_add_filter(GTK_FILE_CHOOSER(dialog), 
		pattern_filter("Stage9 Map", "*.s9m"));
	gtk_file_chooser_add_filter(GTK_FILE_CHOOSER(dialog), 
		pattern_filter("All Files", "*"));
	if(gtk_dialog_run(GTK_DIALOG(dialog)) == GTK_RESPONSE_ACCEPT) {
		char *fn = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(dialog));
		filename = malloc(strlen(fn) + 1);
		strcpy(filename, fn);
		g_free(fn);
	}
	gtk_widget_destroy(dialog);
	while(gtk_events_pending()) gtk_main_iteration();
	return filename;
}

char* dialog_tileset_open() {
	char *filename = NULL;
	GtkWidget *dialog = gtk_file_chooser_dialog_new("Open Tileset", NULL,
		GTK_FILE_CHOOSER_ACTION_OPEN, GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL,
		GTK_STOCK_OPEN, GTK_RESPONSE_ACCEPT, NULL);
	gtk_file_chooser_add_filter(GTK_FILE_CHOOSER(dialog), 
		pattern_filter("All Files", "*"));
	if(gtk_dialog_run(GTK_DIALOG (dialog)) == GTK_RESPONSE_ACCEPT) {
		char *fn = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(dialog));
		filename = malloc(strlen(fn) + 1);
		strcpy(filename, fn);
		g_free(fn);
	}
	gtk_widget_destroy(dialog);
	while(gtk_events_pending()) gtk_main_iteration();
	return filename;
}

char* dialog_tileattr_open() {
	char *filename = NULL;
	GtkWidget *dialog = gtk_file_chooser_dialog_new("Open Tile Attributes", NULL,
		GTK_FILE_CHOOSER_ACTION_OPEN, GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL,
		GTK_STOCK_OPEN, GTK_RESPONSE_ACCEPT, NULL);
	gtk_file_chooser_add_filter(GTK_FILE_CHOOSER(dialog), 
		pattern_filter("Stage9 Attributes", "*.s9a"));
	gtk_file_chooser_add_filter(GTK_FILE_CHOOSER(dialog), 
		pattern_filter("All Files", "*"));
	if(gtk_dialog_run(GTK_DIALOG(dialog)) == GTK_RESPONSE_ACCEPT) {
		char *fn = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(dialog));
		filename = malloc(strlen(fn) + 1);
		strcpy(filename, fn);
		g_free(fn);
	}
	gtk_widget_destroy(dialog);
	while(gtk_events_pending()) gtk_main_iteration();
	return filename;
}

char* dialog_map_save() {
	char *filename = NULL;
	GtkWidget *dialog = gtk_file_chooser_dialog_new("Save Map", NULL,
		GTK_FILE_CHOOSER_ACTION_SAVE, GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL,
		GTK_STOCK_SAVE, GTK_RESPONSE_ACCEPT, NULL);
	gtk_file_chooser_set_do_overwrite_confirmation(GTK_FILE_CHOOSER(dialog), TRUE);
	gtk_file_chooser_add_filter(GTK_FILE_CHOOSER(dialog), 
		pattern_filter("Stage9 Map", "*.s9m"));
	gtk_file_chooser_add_filter(GTK_FILE_CHOOSER(dialog), 
		pattern_filter("All Files", "*"));
	if(gtk_dialog_run(GTK_DIALOG (dialog)) == GTK_RESPONSE_ACCEPT) {
		char *fn = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(dialog));
		filename = malloc(strlen(fn) + 1);
		strcpy(filename, fn);
		g_free(fn);
	}
	gtk_widget_destroy(dialog);
	while(gtk_events_pending()) gtk_main_iteration();
	return filename;
}

char* dialog_tileattr_save() {
	char *filename = NULL;
	GtkWidget *dialog = gtk_file_chooser_dialog_new("Save Tile Attributes", NULL,
		GTK_FILE_CHOOSER_ACTION_SAVE, GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL,
		GTK_STOCK_SAVE, GTK_RESPONSE_ACCEPT, NULL);
	gtk_file_chooser_set_do_overwrite_confirmation(GTK_FILE_CHOOSER(dialog), TRUE);
	gtk_file_chooser_add_filter(GTK_FILE_CHOOSER(dialog), 
		pattern_filter("Stage9 Attributes", "*.s9a"));
	gtk_file_chooser_add_filter(GTK_FILE_CHOOSER(dialog), 
		pattern_filter("All Files", "*"));
	if(gtk_dialog_run(GTK_DIALOG (dialog)) == GTK_RESPONSE_ACCEPT) {
		char *fn = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(dialog));
		filename = malloc(strlen(fn) + 1);
		strcpy(filename, fn);
		g_free(fn);
	}
	gtk_widget_destroy(dialog);
	while(gtk_events_pending()) gtk_main_iteration();
	return filename;
}
