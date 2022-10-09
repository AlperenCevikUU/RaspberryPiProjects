#include <stdio.h>
#include <stdlib.h>
#include <wiringPi.h>
#include "gtk/gtk.h"

#define TRIG 5
#define ECHO 6

guint threadID = 0;
guint serial_counter = 0;

  GtkWidget	*fixed;
  GtkWidget *window;
  GtkWidget *halign;
  GtkWidget *hbox;
  GtkWidget *hscale;
  GtkWidget *label;
  GtkWidget *start_button;
  GtkWidget *stop_button;
  GtkWidget *Ulabel;

void setup() {
        wiringPiSetup();
        pinMode(TRIG, OUTPUT);
        pinMode(ECHO, INPUT);

        //TRIG pin must start LOW
        digitalWrite(TRIG, LOW);
        delay(30);
}
void value_changed(GtkRange *range, gpointer win) {
    
   gdouble val = gtk_range_get_value(range);
   gchar *str = g_strdup_printf("%.f", val);    
   gtk_label_set_text(GTK_LABEL(win), str);
   
   g_free(str);
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


static gboolean
serial_data (gpointer user_data)
{
    
   
    gtk_range_set_value(hscale, getCM());
   
  
    printf("Mesafe: %dcm\n\r", getCM());
    return user_data;
}

static void
on_start_button_clicked (GtkButton* button, gpointer user_data)
{
    if (user_data == 1)
    {
        threadID = g_timeout_add(1000, serial_data, user_data);
    }
    else if (user_data == 0)
    {
        g_source_remove(threadID);
        threadID = 0;
        
    }
}


	int main(int argc, char *argv[]) {

  setup();
        
      

  gtk_init(&argc, &argv);

//gtk_css_provider_load_from_path(GtkCssProvider, "renk.css", NULL);

  window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
  gtk_window_set_position(GTK_WINDOW(window), GTK_WIN_POS_CENTER);
  gtk_window_set_default_size(GTK_WINDOW(window), 300, 250);
  gtk_container_set_border_width(GTK_CONTAINER(window), 10);
  gtk_window_set_title(GTK_WINDOW(window), "Mesafe Ölçer");
  fixed = gtk_fixed_new();
  
  hbox = gtk_hbox_new(FALSE, 20);

  hscale = gtk_hscale_new_with_range(0, 200, 1);
  gtk_scale_set_draw_value(GTK_SCALE(hscale), FALSE);
  gtk_widget_set_size_request(hscale, 250, -1);
  label = gtk_label_new("...");
  Ulabel = gtk_label_new("Mesafe(cm)");
  gtk_misc_set_alignment(GTK_MISC(label), 0.0, 1);
  gtk_fixed_put(GTK_FIXED(fixed),Ulabel,0,0);
  
  start_button = gtk_button_new_with_label ("Start");
  gtk_fixed_put(GTK_FIXED(fixed),start_button,80,100);
  gtk_widget_set_size_request(start_button,80,30);
  
  stop_button = gtk_button_new_with_label ("Stop");
  gtk_fixed_put(GTK_FIXED(fixed),stop_button,180,100);
  gtk_widget_set_size_request(stop_button,80,30);
  
  gtk_box_pack_start(GTK_BOX(hbox), hscale, FALSE, FALSE, 0);
  gtk_box_pack_start(GTK_BOX(hbox), label, FALSE, FALSE, 0);
  
  gtk_container_add(GTK_CONTAINER(window), fixed);

  halign = gtk_alignment_new(0, 0, 0, 0);
  
  gtk_container_add(GTK_CONTAINER(halign), hbox);
  gtk_fixed_put(GTK_FIXED(fixed),halign,5,20);

  g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);
  g_signal_connect(hscale, "value-changed", G_CALLBACK(value_changed), label);
  g_signal_connect (start_button, "clicked", G_CALLBACK(on_start_button_clicked), 1);
  g_signal_connect (stop_button, "clicked", G_CALLBACK(on_start_button_clicked), 0);    

  gtk_widget_show_all(window);

  gtk_main();

  return 0;
}
        



