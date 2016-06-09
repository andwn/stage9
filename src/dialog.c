#include "dialog.h"
#include <gtk/gtk.h>

GtkFileFilter* pattern_filter(const char *name, const char *pattern) {
	GtkFileFilter *filter = gtk_file_filter_new();
	gtk_file_filter_set_name(filter, name);
	gtk_file_filter_add_pattern(filter, pattern);
	return filter;
}

char* dialog_open_map() {
	char *filename = NULL;
	GtkWidget *dialog = gtk_file_chooser_dialog_new("Open Map", NULL,
		GTK_FILE_CHOOSER_ACTION_OPEN, GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL,
		GTK_STOCK_OPEN, GTK_RESPONSE_ACCEPT, NULL);
	gtk_file_chooser_add_filter(GTK_FILE_CHOOSER(dialog), 
		pattern_filter("Stage9 Map", "*.s9m"));
	gtk_file_chooser_add_filter(GTK_FILE_CHOOSER(dialog), 
		pattern_filter("RPG Maker Map", "*.lmu"));
	gtk_file_chooser_add_filter(GTK_FILE_CHOOSER(dialog), 
		pattern_filter("All Files", "*"));
	if(gtk_dialog_run(GTK_DIALOG (dialog)) == GTK_RESPONSE_ACCEPT) {
		filename = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER (dialog));
		//OpenDeck(filename);
		//g_free(filename);
	}
	gtk_widget_destroy(dialog);
	while(gtk_events_pending()) gtk_main_iteration();
	return filename;
}

char* dialog_open_tileset() {
	
}

char* dialog_open_tileprop() {
	
}

char* dialog_save_map(const char *defaultFilename) {
	char *filename = NULL;
	GtkWidget *dialog = gtk_file_chooser_dialog_new("Save Map", NULL,
		GTK_FILE_CHOOSER_ACTION_SAVE, GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL,
		GTK_STOCK_SAVE, GTK_RESPONSE_ACCEPT, NULL);
	gtk_file_chooser_set_do_overwrite_confirmation(GTK_FILE_CHOOSER(dialog), TRUE);
	if(defaultFilename != NULL) {
		gtk_file_chooser_set_current_name(GTK_FILE_CHOOSER(dialog), defaultFilename);
	}
	gtk_file_chooser_add_filter(GTK_FILE_CHOOSER(dialog), 
		pattern_filter("Stage9 Map", "*.s9m"));
	gtk_file_chooser_add_filter(GTK_FILE_CHOOSER(dialog), 
		pattern_filter("All Files", "*"));
	if(gtk_dialog_run(GTK_DIALOG (dialog)) == GTK_RESPONSE_ACCEPT) {
		filename = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(dialog));
		//SaveDeck(filename);
		//strcpy(deckname, filename);
		//g_free(filename);
	}
	gtk_widget_destroy(dialog);
	while(gtk_events_pending()) gtk_main_iteration();
	return filename;
}

char* dialog_save_tileprop() {
	
}
