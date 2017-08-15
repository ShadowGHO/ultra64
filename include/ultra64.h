#ifndef _ULTRA64_H
#define _ULTRA64_H

class MainWindow: public wxFrame
{
  public:
    MainWindow(const wxString &title, const wxPoint &pos, const wxSize &size);
  private:
    void OnOpenROM(wxCommandEvent &event);
    void OnOpenPIFROM(wxCommandEvent &event);
    void OnExit(wxCommandEvent &event);
    void OnAbout(wxCommandEvent &event);
    void OnDebugPIFROM(wxCommandEvent &event);
    void OnDebugROM(wxCommandEvent &event);
    void OnDebugRegisters(wxCommandEvent &event);
    wxDECLARE_EVENT_TABLE();
};

class RegistersWindow: public wxFrame
{
  public:
    RegistersWindow(wxWindow *parent, const wxString &title, const wxPoint &pos, const wxSize &size);
    ~RegistersWindow();
    wxListBox *registers_listbox = NULL;
};

class DebuggerWindow: public wxFrame
{
  public:
    DebuggerWindow(wxWindow *parent, const wxString &title, const wxPoint &pos, const wxSize &size);
    ~DebuggerWindow();
    wxListBox *memory_listbox = NULL;
    void view(uint32_t start_addr, uint32_t end_addr);
    wxButton *cpu_step = NULL;
    void OnCPUStep(wxCommandEvent &event);
    wxDECLARE_EVENT_TABLE();
};

class wxUltra64: public wxApp
{
  public:
    virtual bool OnInit();
    MainWindow *frame = NULL;
    DebuggerWindow *debugger = NULL;
    RegistersWindow *registers = NULL;
};

enum
{
    ID_open_pif_rom = 0,
    ID_open_rom = 1,

    ID_debug_pif_rom = 10,
    ID_debug_rom = 11,
    ID_debug_registers = 12,

    BUTTON_cpu_step = 20
};

const int ID_MEMORY_LISTBOX = 100;
const int ID_REGISTERS_LISTBOX = 101;
#endif
