#include <Godot.hpp>
#include <File.hpp>
#include <Reference.hpp>
#include <Array.hpp>

#include <vector>
#include <stdio.h>

using namespace godot;

typedef std::pair<int, int> note_t;

class MidiReader : public Reference {
  GODOT_CLASS(MidiReader, Reference);
  Ref<File> file;
  std::vector<note_t> notes;
  
public:
  void _init() {}

  void open(String filename) {
    int length;
    int track_length;
    int track_end;
    int time = 0;
    int delta_time;
    int err;
    
    file = (Ref<File>) File::_new();
    file->set_endian_swap(true); // MIDI is Big Endian
    err = (int) file->open(filename, File::ModeFlags::READ);
    if (err) {
      Godot::print("Could not load " + filename);
      return;
    }

    // Read the headers
    offset_seek(4); // Skip MThd
    length = file->get_32(); // Get the length of the headers
    offset_seek(length); // Skip the headers for now

    // Read the first track (Should be guitar track)
    offset_seek(4); // Skip MTrk
    track_length = file->get_32(); // Get the length of the track in bytes
    track_end = file->get_position() + track_length;
    // Loop over the track data
    unsigned char running_status;
    while (track_end > file->get_position()) {
      int delta;
      delta = read_variable_length();
      std::cout << "Delta time: " << delta << std::endl;
      time += delta;
      

      unsigned char lead_in = file->get_8();
    running_status_reenter:
      if (lead_in == 0xFF) {
	// This is a meta-event
	unsigned char type = file->get_8();
	length = read_variable_length();
	offset_seek(length);
	// Skipping for now, but we should really read these
      } else {
	int pitch, velocity;
	switch(lead_in & 0xF0) {
	case 0x80: // Note off
	  offset_seek(2); // Skip this for now
	  break;
	case 0x90: // Note on
	  pitch = file->get_8();
	  velocity = file->get_8();
	  if (velocity > 0) // Some midi files use 0 velocity to signify note off
	    notes.push_back(note_t(time, pitch));
	  break;
	case 0xA0: // Polyphonic Key Pressure
	case 0xB0: // Control Change
	case 0xE0: // Pitch Wheel Change
	  offset_seek(2);
	  break;
	case 0xC0: // Program change
	case 0xD0: // After-touch
	  offset_seek(1);
	  break;
	default:
	  // Running status (previous event is the same as the current)
	  // Go back a byte and reenter;
	  offset_seek(-1);
	  lead_in = running_status;
	  goto running_status_reenter;
	}
      }
      running_status = lead_in;
    }

    Godot::print("Loaded " + filename);
  }

  void close() {
    file->close();
  }

  int read_variable_length(int *nbytes=NULL) {
    int val = 0, bytes = 0;
    unsigned char last;
    do {
      bytes++;
      last = file->get_8();
      val = last & 0x7F;
    } while (last & 0x80);

    if (nbytes) {
      *nbytes = bytes;
    }

    return val;
  }

  Array get_notes_in_range(int start, int end) {
    Array arr;
    for (note_t note : notes) {
      if (note.first >= start and note.first < end) {
	arr.push_back(note.second);
      }
    }
    
    return arr;
  }

  Array get_all_notes() {
    Array arr;
    for (note_t note : notes) {
      arr.push_back(note.second);
    }

    return arr;
  }

  void offset_seek(int offset) {
    // Seek for an offset of the current position
    int pos = file->get_position();
    file->seek(pos + offset);
  }
  
  static void _register_methods() {
    register_method("open", &MidiReader::open);
    register_method("close", &MidiReader::close);
    register_method("get_notes_in_range", &MidiReader::get_notes_in_range);
    register_method("get_all_notes", &MidiReader::get_all_notes);
  }
};


extern "C" void GDN_EXPORT godot_gdnative_init(godot_gdnative_init_options *o) {
  godot::Godot::gdnative_init(o);
}

extern "C" void GDN_EXPORT godot_gdnative_terminate(godot_gdnative_terminate_options *o) {
  godot::Godot::gdnative_terminate(o);
}

extern "C" void GDN_EXPORT godot_nativescript_init(void *handle) {
  godot::Godot::nativescript_init(handle);
  godot::register_class<MidiReader>();
}

