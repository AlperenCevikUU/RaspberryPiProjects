#include <stdio.h>
#include <stdlib.h>
#include <wiringPi.h>
#include <gtk/gtk.h>

#define TRIG 22 // 5
#define ECHO 25 // 6

char us[15];

GtkWidget *window, *label, *button, *vbox;

void setup() {
        wiringPiSetup();
        pinMode(TRIG, OUTPUT);
        pinMode(ECHO, INPUT);

        //TRIG pin must start LOW
        digitalWrite(TRIG, LOW);
        delay(30);
}

int getCM() {
        //Send trig pulse
        digitalWrite(TRIG, HIGH);
        delayMicroseconds(20);
        digitalWrite(TRIG, LOW);

        //Wait for echo start
        while(digitalRead(ECHO) == LOW);

        //Wait for echo end
        long startTime = micros();
        while(digitalRead(ECHO) == HIGH);
        long travelTime = micros() - startTime;

        //Get distance in cm
        int distance = travelTime / 58;

        return distance;
}

static void button_clicked (GtkWidget *widget, gpointer data) // BUTTON
{
	sprintf(us, "Distance : %dCM", getCM());
	gtk_label_set_text(GTK_LABEL(label),(const gchar*)us);
}

int main(int argc, char *argv[])
{
        setup();
        gtk_init(&argc,&argv);
        
        window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	label = gtk_label_new("Distance in CM");
	button = gtk_button_new_with_label("Read Distance");
	g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);
	g_signal_connect(button, "clicked", G_CALLBACK(button_clicked), NULL);
	vbox = gtk_box_new(1,0);
	gtk_box_pack_start(GTK_BOX(vbox), label, 1, 1, 0);
	gtk_box_pack_start(GTK_BOX(vbox), button, 1, 1, 0);

	gtk_container_add(GTK_CONTAINER (window), vbox);
	GtkCssProvider *provider=gtk_css_provider_new();
	gtk_css_provider_load_from_path(provider, "widgets.css", NULL);

	GtkStyleContext *context1, *context2, *context3;
	context1=gtk_widget_get_style_context(button);
	context2=gtk_widget_get_style_context(window);
	context3=gtk_widget_get_style_context(label);

	gtk_style_context_add_provider (context1,
                                        GTK_STYLE_PROVIDER(provider),
                                        GTK_STYLE_PROVIDER_PRIORITY_USER);
								
	gtk_style_context_add_provider (context2,
                                        GTK_STYLE_PROVIDER(provider),
                                        GTK_STYLE_PROVIDER_PRIORITY_USER);

	gtk_style_context_add_provider (context3,
                                        GTK_STYLE_PROVIDER(provider),
                                        GTK_STYLE_PROVIDER_PRIORITY_USER);
                                        
	gtk_widget_show_all(window);
	gtk_main();

        return 0;
}