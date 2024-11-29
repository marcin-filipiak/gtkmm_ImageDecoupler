#include <gtkmm.h>
#include <iostream>
#include <fstream>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <zlib.h>
#include <vector>
#include <cstdio> // std::remove

#define VERSION "0.4"

class DecouplerApp : public Gtk::Window {
public:
    DecouplerApp() {
        set_title("DECOUPLER - Find and Remove Duplicated JPG");
        set_default_size(600, 400);

        // Layout
        vbox.set_spacing(10);
        vbox.set_margin_start(10);
        vbox.set_margin_end(10);
        vbox.set_margin_top(10);
        vbox.set_margin_bottom(10);
        add(vbox);

        // File chooser button
        btnChooseFolder.set_label("Choose Folder");
        btnChooseFolder.signal_clicked().connect(sigc::mem_fun(*this, &DecouplerApp::on_choose_folder));
        vbox.pack_start(btnChooseFolder, Gtk::PACK_SHRINK);

        // Recursive checkbox
        chkRecursive.set_label("Search Recursively");
        vbox.pack_start(chkRecursive, Gtk::PACK_SHRINK);

        // Process button
        btnProcess.set_label("Find Duplicates");
        btnProcess.signal_clicked().connect(sigc::mem_fun(*this, &DecouplerApp::on_process_clicked));
        vbox.pack_start(btnProcess, Gtk::PACK_SHRINK);

        // Image preview
        vbox.pack_start(imgPreview, Gtk::PACK_SHRINK);

        // Buttons for duplicate handling, arranged vertically
        btnBox.set_orientation(Gtk::ORIENTATION_VERTICAL); // Change to vertical layout for buttons
        btnBox.set_spacing(10);
        vbox.pack_start(btnBox, Gtk::PACK_SHRINK);

        // Initialize the duplicate buttons
        btnFile1.signal_clicked().connect(sigc::mem_fun(*this, &DecouplerApp::on_file1_clicked));
        btnFile2.signal_clicked().connect(sigc::mem_fun(*this, &DecouplerApp::on_file2_clicked));

        // Make sure buttons are added at the start
        btnBox.pack_start(btnFile1, Gtk::PACK_SHRINK);
        btnBox.pack_start(btnFile2, Gtk::PACK_SHRINK);

        show_all_children();
    }
    
   ~DecouplerApp() {
        // Delete the decoupler_list.txt file when the application is closed
        std::string outputPath = folderPath + "/decoupler_list.txt";
        std::remove(outputPath.c_str());
    }

private:
    Gtk::Box vbox{Gtk::ORIENTATION_VERTICAL};
    Gtk::Box btnBox{Gtk::ORIENTATION_VERTICAL}; // Use vertical layout for buttons
    Gtk::Button btnChooseFolder, btnProcess, btnFile1, btnFile2; // Declare btnFile1 and btnFile2
    Gtk::CheckButton chkRecursive;
    Gtk::Image imgPreview;

    std::string folderPath;
    std::vector<std::pair<std::string, std::string>> duplicates;
    size_t currentDuplicateIndex = 0;

    void on_choose_folder() {
        Gtk::FileChooserDialog dialog(*this, "Choose Folder", Gtk::FileChooserAction::FILE_CHOOSER_ACTION_SELECT_FOLDER);
        dialog.add_button("Cancel", Gtk::ResponseType::RESPONSE_CANCEL);
        dialog.add_button("Select", Gtk::ResponseType::RESPONSE_OK);

        if (dialog.run() == Gtk::ResponseType::RESPONSE_OK) {
            folderPath = dialog.get_filename();
            std::cout << "Folder selected: " << folderPath << std::endl;
        }
    }

    void on_process_clicked() {
        if (folderPath.empty()) {
            Gtk::MessageDialog dialog(*this, "Please select a folder first.");
            dialog.run();
            return;
        }

        std::string outputPath = folderPath + "/decoupler_list.txt";
        std::remove(outputPath.c_str());  // Delete the file if it exists

        // Save JPG paths and CRC32 hashes
        if (chkRecursive.get_active()) {
            saveJPGFileThisPaths(folderPath, outputPath);
        } else {
            saveJPGFileThisPaths(folderPath, outputPath);
        }

        findDuplicateCRC32(outputPath);
    }

    void saveJPGFileThisPaths(const std::string& folderPath, const std::string& outputPath) {
        std::ofstream outputFile(outputPath);
        if (!outputFile.is_open()) {
            std::cerr << "Unable to open output file!" << std::endl;
            return;
        }

        DIR* dir;
        struct dirent* entry;
        if ((dir = opendir(folderPath.c_str())) != nullptr) {
            while ((entry = readdir(dir)) != nullptr) {
                std::string fileName = entry->d_name;
                if (fileName.length() >= 4 && fileName.substr(fileName.length() - 4) == ".jpg") {
                    std::string fp = folderPath + '/' + fileName;
                    outputFile << "\"" << fp << "\" \"" << calculateCRC32(fp) << "\"" << std::endl;
                }
            }
            closedir(dir);
        } else {
            std::cerr << "Unable to open directory!" << std::endl;
            return;
        }

        outputFile.close();
    }

    std::string calculateCRC32(const std::string& filePath) {
        std::ifstream file(filePath, std::ios::binary);
        if (!file) {
            std::cerr << "Error: Unable to open file " << filePath << std::endl;
            return "";
        }

        uLong crc = crc32(0L, Z_NULL, 0);
        const int bufferSize = 1024;
        char buffer[bufferSize];
        while (file.good()) {
            file.read(buffer, bufferSize);
            crc = crc32(crc, reinterpret_cast<const Bytef*>(buffer), file.gcount());
        }
        file.close();

        std::stringstream ss;
        ss << std::hex << crc;
        return ss.str();
    }

    void findDuplicateCRC32(const std::string& filePath) {
        std::ifstream file(filePath);
        if (!file) {
            std::cerr << "Error: Unable to open file " << filePath << std::endl;
            return;
        }

        std::unordered_map<std::string, std::string> crcMap;
        std::unordered_set<std::string> processedFiles;
        std::string line;
        while (std::getline(file, line)) {
            std::istringstream iss(line);
            std::string filePath, crcHash;

            std::getline(iss, filePath, '"');
            std::getline(iss, filePath, '"');
            std::getline(iss, crcHash, '"');
            std::getline(iss, crcHash, '"');

            if (processedFiles.count(filePath) == 0) {
                if (!crcMap.count(crcHash)) {
                    crcMap[crcHash] = filePath;
                } else {
                    duplicates.push_back({crcMap[crcHash], filePath});
                    processedFiles.insert(filePath);
                }
            }
        }

        file.close();

        if (duplicates.empty()) {
            Gtk::MessageDialog dialog(*this, "No duplicates found!");
            dialog.run();
        } else {
            load_next_duplicate();
        }
    }

void load_next_duplicate() {
    if (currentDuplicateIndex < duplicates.size()) {
        const auto& dup = duplicates[currentDuplicateIndex];

        // Ładowanie podglądu obrazu dla pierwszego pliku
        try {
            std::cout << "Trying to load file: " << dup.first << std::endl;
            auto pixbuf = Gdk::Pixbuf::create_from_file(dup.first);

            // Skaluje obraz, aby pasował do okna, zachowując proporcje
            int original_width = pixbuf->get_width();
            int original_height = pixbuf->get_height();
            int new_width = (400 * original_width) / original_height; // Zachowuje proporcje

            pixbuf = pixbuf->scale_simple(new_width, 400, Gdk::INTERP_BILINEAR); // Zmienia rozmiar na 400px wysokości

            imgPreview.set(pixbuf);
        } catch (const std::exception& e) {
            std::cerr << "Error loading file: " << e.what() << std::endl;
        }

        // Zaktualizowanie etykiet przycisków z nazwami plików
        btnFile1.set_label(dup.first);
        btnFile2.set_label(dup.second);

        currentDuplicateIndex++;
    } else {
        Gtk::MessageDialog dialog(*this, "No more duplicates!");
        dialog.run();
    }
}


void on_file1_clicked() {
    // Handle deletion of the first file
    const auto& dup = duplicates[currentDuplicateIndex - 1];
    if (deleteFile(dup.first)) {
        load_next_duplicate(); // Przechodzi do następnego duplikatu
    }
}

void on_file2_clicked() {
    // Handle deletion of the second file
    const auto& dup = duplicates[currentDuplicateIndex - 1];
    if (deleteFile(dup.second)) {
        load_next_duplicate(); // Przechodzi do następnego duplikatu
    }
}

    bool deleteFile(const std::string& filePath) {
        if (std::remove(filePath.c_str()) != 0) {
            std::cerr << "Error: Unable to delete file " << filePath << std::endl;
            return false;
        }
        std::cout << "File deleted: " << filePath << std::endl;
        return true;
    }
};

int main(int argc, char* argv[]) {
    Gtk::Main kit(argc, argv);
    DecouplerApp window;
    Gtk::Main::run(window);
    return 0;
}

