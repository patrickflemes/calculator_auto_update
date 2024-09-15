#include <gtk/gtk.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <cstdlib>
#include <iomanip>

#define CURRENT_VERSION "1.0"

static GtkWidget *entry;

std::string read_version_from_file() {
    std::ifstream file("versao.txt");
    std::string line;
    if (file.is_open()) {
        std::getline(file, line);
        file.close();
    } else {
        std::cerr << "Não foi possível abrir o arquivo versao.txt.\n";
    }
    return line;
}

bool is_version_newer(const std::string& current, const std::string& latest) {
    std::stringstream ss_current(current);
    std::stringstream ss_latest(latest);
    int curr_major, curr_minor, latest_major, latest_minor;
    char dot;

    ss_current >> curr_major >> dot >> curr_minor;
    ss_latest >> latest_major >> dot >> latest_minor;

    if (latest_major > curr_major) {
        return true;
    } else if (latest_major == curr_major && latest_minor > curr_minor) {
        return true;
    }
    return false;
}

static void check_for_updates() {
    std::string latest_version = read_version_from_file();

    if (latest_version.empty()) {
        std::cerr << "Não foi possível ler a versão mais recente.\n";
        return;
    }

    if (is_version_newer(CURRENT_VERSION, latest_version)) {
        std::cout << "Atualizando para a versão " << latest_version << "...\n";
        if (std::system("./update_calculator.sh &") != 0) {
            std::cerr << "Erro ao executar o script de atualização.\n";
            return;
        }
        exit(0);
    }
}


double evaluate_expression(const std::string& expr) {
    std::stringstream ss(expr);
    double left_operand, right_operand;
    char op;

    ss >> left_operand;
    ss >> op;
    ss >> right_operand;

    if (ss.fail()) {
        std::cerr << "Erro ao avaliar a expressão.\n";
        return 0.0;
    }

    switch(op) {
        case '+':
            return left_operand + right_operand;
        case '-':
            return left_operand - right_operand;
        case '*':
            return left_operand * right_operand;
        case '/':
            if (right_operand != 0)
                return left_operand / right_operand;
            else {
                std::cerr << "Erro: Divisão por zero.\n";
                return 0.0;
            }
        default:
            std::cerr << "Operador desconhecido: " << op << "\n";
            return 0.0;
    }
}

static void button_clicked(GtkWidget *widget, gpointer data) {
    const gchar *button_label = gtk_button_get_label(GTK_BUTTON(widget));
    const gchar *current_text = gtk_entry_get_text(GTK_ENTRY(entry));

    if(g_strcmp0(button_label, "=") == 0) {
        std::string expression(current_text);
        double result = evaluate_expression(expression);

        std::ostringstream result_stream;
        result_stream << std::fixed << std::setprecision(2) << result;
        std::string result_str = result_stream.str();

        gtk_entry_set_text(GTK_ENTRY(entry), result_str.c_str());
    } else if(g_strcmp0(button_label, "C") == 0) {
        gtk_entry_set_text(GTK_ENTRY(entry), "");
    } else {
        std::string new_text = std::string(current_text) + button_label;
        gtk_entry_set_text(GTK_ENTRY(entry), new_text.c_str());
    }
}

int main(int argc, char *argv[]) {
    gtk_init(&argc, &argv);

    check_for_updates();

    GtkWidget *window;
    GtkWidget *grid;
    GtkWidget *button;

    window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(window), "Calculadora");
    gtk_window_set_default_size(GTK_WINDOW(window), 250, 300);
    gtk_container_set_border_width(GTK_CONTAINER(window), 10);

    g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);

    entry = gtk_entry_new();
    gtk_entry_set_alignment(GTK_ENTRY(entry), 1);
    gtk_widget_set_hexpand(entry, TRUE);
    gtk_widget_set_vexpand(entry, FALSE);

    grid = gtk_grid_new();
    gtk_grid_set_row_spacing(GTK_GRID(grid), 5);
    gtk_grid_set_column_spacing(GTK_GRID(grid), 5);

    gtk_grid_set_row_homogeneous(GTK_GRID(grid), TRUE);
    gtk_grid_set_column_homogeneous(GTK_GRID(grid), TRUE);

    gtk_container_add(GTK_CONTAINER(window), grid);
    gtk_grid_attach(GTK_GRID(grid), entry, 0, 0, 4, 1);

    const char *buttons[4][4] = {
        {"7", "8", "9", "/"},
        {"4", "5", "6", "*"},
        {"1", "2", "3", "-"},
        {"0", "C", "=", "+"}
    };

    for(int i = 0; i < 4; ++i) {
        for(int j = 0; j < 4; ++j) {
            button = gtk_button_new_with_label(buttons[i][j]);

            gtk_widget_set_hexpand(button, TRUE);
            gtk_widget_set_vexpand(button, TRUE);

            gtk_grid_attach(GTK_GRID(grid), button, j, i + 1, 1, 1);
            g_signal_connect(button, "clicked", G_CALLBACK(button_clicked), NULL);
        }
    }

    gtk_widget_show_all(window);
    gtk_main();

    return 0;
}

