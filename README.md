# DECOUPLER - Find and Remove Duplicated JPG Files

## Overview

DECOUPLER is a desktop application designed to help users find and remove duplicate JPG files from a selected directory or its subdirectories. By calculating and comparing the CRC32 hash of each file, the program identifies duplicates and allows the user to decide which file to delete.

This tool can be particularly useful for users who need to clean up their photo libraries, manage disk space, or organize image collections.

## Features

- **Folder Selection**: Choose a folder to search for JPG files.
- **Recursive Search**: Option to include subdirectories in the search.
- **Find Duplicates**: Automatically detects duplicate JPG files based on their CRC32 hash.
- **File Deletion**: Allows the user to delete duplicates directly from the interface.
- **Image Preview**: Displays a preview of the duplicate images for easier decision-making.
- **File Management**: Generate a list of JPG files with their respective CRC32 hashes and use it for further analysis.

## How it Works

1. **Select a Folder**: Use the "Choose Folder" button to select the folder to search for duplicate JPG files.
2. **Search for Duplicates**: Click "Find Duplicates" to start the search. The program will scan the selected folder (and optionally, its subdirectories) to find JPG files and calculate their CRC32 hashes.
3. **Handle Duplicates**: If duplicates are found, the program will display them with a preview of the images. Users can then choose to delete either file in the duplicate pair.
4. **Manage Results**: The application generates a `decoupler_list.txt` file containing the paths and CRC32 hashes of the files, which can be used for further analysis or record-keeping.

## Usage

1. Launch the application.
2. Click the "Choose Folder" button to select a directory.
3. Check the "Search Recursively" box if you want to include subdirectories.
4. Click "Find Duplicates" to begin scanning the folder for duplicates.
5. If duplicates are found, you can preview and delete them using the provided buttons.

## System Requirements

- **Operating System**: Linux (Debian-based recommended)
- **Dependencies**:
  - GTKmm (for the graphical interface)
  - zlib (for CRC32 hash calculation)

## License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.
