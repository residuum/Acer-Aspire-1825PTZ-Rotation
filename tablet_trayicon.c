#include "tablet_trayicon.h"

void tray_icon_on_click(GtkStatusIcon *status_icon, gpointer user_data) {
	system("tabletmode auto");
}

GtkStatusIcon *create_tray_icon() {
	GtkStatusIcon *tray_icon;

	tray_icon = gtk_status_icon_new();
	g_signal_connect(G_OBJECT(tray_icon), "activate", G_CALLBACK(tray_icon_on_click), NULL);
	gtk_status_icon_set_from_icon_name(tray_icon, GTK_STOCK_MEDIA_STOP);
	gtk_status_icon_set_tooltip(tray_icon, "Tablet mode switcher");
	gtk_status_icon_set_visible(tray_icon, TRUE);

	return tray_icon;
}

int main(int argc, char **argv) {
	GtkStatusIcon *tray_icon;

	gtk_init(&argc, &argv);
	tray_icon = create_tray_icon();
	gtk_main();

	return 0;
}
