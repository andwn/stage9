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

char* dialog_map_open() {
	char *filename = NULL;
	GtkWidget *dialog = gtk_file_chooser_dialog_new("Open Map", NULL,
		GTK_FILE_CHOOSER_ACTION_OPEN, GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL,
		GTK_STOCK_OPEN, GTK_RESPONSE_ACCEPT, NULL);
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
	return NULL;
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
	return NULL;
}
