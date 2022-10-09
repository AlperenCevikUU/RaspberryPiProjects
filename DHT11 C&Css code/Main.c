#include <wiringPi.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <gtk/gtk.h>

#define MAXTIMINGS	85
#define DHTPIN		7
int dht11_dat[5] = { 0, 0, 0, 0, 0 };
char nem[15];
char isi[15];

GtkWidget *window, *label, *label1, *button, *vbox;
 
void read_dht11_dat()
{
	uint8_t laststate	= HIGH;
	uint8_t counter		= 0;
	uint8_t j		= 0, i;
	
	dht11_dat[0] = dht11_dat[1] = dht11_dat[2] = dht11_dat[3] = dht11_dat[4] = 0;
	
	pinMode( DHTPIN, OUTPUT );
	digitalWrite( DHTPIN, LOW );
	delay( 18 );
	digitalWrite( DHTPIN, HIGH );
	delayMicroseconds( 40 );
	pinMode( DHTPIN, INPUT );
 
	for ( i = 0; i < MAXTIMINGS; i++ )
	{
		counter = 0;
		while ( digitalRead( DHTPIN ) == laststate )
		{
			counter++;
			delayMicroseconds( 1 );
			if ( counter == 255 )
			{
				break;
			}
		}
		laststate = digitalRead( DHTPIN );
 
		if ( counter == 255 )
			break;
 
		if ( (i >= 4) && (i % 2 == 0) )
		{
			dht11_dat[j / 8] <<= 1;
			if ( counter > 50 )
				dht11_dat[j / 8] |= 1;
			j++;
		}
	if ( (j >= 40) &&
	     (dht11_dat[4] == ( (dht11_dat[0] + dht11_dat[1] + dht11_dat[2] + dht11_dat[3]) & 0xFF) ) )
	{
		sprintf(isi, "Sıcaklık: %d.%dC", dht11_dat[2],dht11_dat[3]);
		sprintf(nem, "Nem : %d.%d%%", dht11_dat[0],dht11_dat[1]);
		gtk_label_set_text(GTK_LABEL(label),(const gchar*)isi);
		gtk_label_set_text(GTK_LABEL(label1),(const gchar*)nem);
	}
	}
}

static void button_clicked (GtkWidget *widget, gpointer data) // BUTTON
{
	read_dht11_dat();
}

int main(int argc, char *argv[])
{
	gtk_init(&argc,&argv);
	if ( wiringPiSetup() == -1 )
		exit( 1 );
	
	window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	label = gtk_label_new("Sıcaklık");
	label1 = gtk_label_new("Nem");
	button = gtk_button_new_with_label("Read Value");
	g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);
	g_signal_connect(button, "clicked", G_CALLBACK(read_dht11_dat), NULL);
	vbox = gtk_box_new(1,0);
	gtk_box_pack_start(GTK_BOX(vbox), label, 1, 1, 0);
	gtk_box_pack_start(GTK_BOX(vbox), label1, 1, 1, 0);
	gtk_box_pack_start(GTK_BOX(vbox), button, 1, 1, 0);

	gtk_container_add(GTK_CONTAINER (window), vbox);
	GtkCssProvider *provider=gtk_css_provider_new();
	gtk_css_provider_load_from_path(provider, "widgets.css", NULL);

	GtkStyleContext *context1, *context2, *context3, *context4;
	context1=gtk_widget_get_style_context(button);
	context2=gtk_widget_get_style_context(window);
	context3=gtk_widget_get_style_context(label);
	context4=gtk_widget_get_style_context(label1);

	gtk_style_context_add_provider (context1,
									GTK_STYLE_PROVIDER(provider),
									GTK_STYLE_PROVIDER_PRIORITY_USER);
								
	gtk_style_context_add_provider (context2,
									GTK_STYLE_PROVIDER(provider),
									GTK_STYLE_PROVIDER_PRIORITY_USER);

	gtk_style_context_add_provider (context3,
									GTK_STYLE_PROVIDER(provider),
									GTK_STYLE_PROVIDER_PRIORITY_USER);
	
	gtk_style_context_add_provider (context4,
									GTK_STYLE_PROVIDER(provider),
									GTK_STYLE_PROVIDER_PRIORITY_USER);
		
	gtk_widget_show_all(window);
	gtk_main();
	
	return(0);

}