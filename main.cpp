#include <wx/wx.h>
#include <curl/curl.h>
#include <sstream>
#include "simpleshell.hpp"

class MyApp : public wxApp {
public:
    virtual bool OnInit();
};

class MyFrame : public wxFrame {
public:
    MyFrame(const wxString& title);

    void OnExecute(wxCommandEvent& event);
    void OnOpenChatbot(wxCommandEvent& event); // New event handler

private:
    wxTextCtrl* input;
    wxTextCtrl* output;
    wxDECLARE_EVENT_TABLE();
};

enum {
    ID_Execute = 1,
    ID_OpenChatbot // New ID for the chatbot button
};

wxBEGIN_EVENT_TABLE(MyFrame, wxFrame)
    EVT_BUTTON(ID_Execute, MyFrame::OnExecute)
    EVT_BUTTON(ID_OpenChatbot, MyFrame::OnOpenChatbot) // New event binding
wxEND_EVENT_TABLE()

wxIMPLEMENT_APP(MyApp);

bool MyApp::OnInit() {
    MyFrame* frame = new MyFrame("Simple Shell GUI");
    frame->Show(true);
    return true;
}

MyFrame::MyFrame(const wxString& title)
    : wxFrame(NULL, wxID_ANY, title, wxDefaultPosition, wxSize(500, 400)) {
    wxPanel* panel = new wxPanel(this, wxID_ANY);

    wxBoxSizer* vbox = new wxBoxSizer(wxVERTICAL);

    input = new wxTextCtrl(panel, wxID_ANY, "", wxDefaultPosition, wxSize(480, 30));
    vbox->Add(input, 0, wxALL | wxEXPAND, 10);

    wxButton* executeButton = new wxButton(panel, ID_Execute, "Execute");
    vbox->Add(executeButton, 0, wxALL | wxCENTER, 10);

    wxButton* chatbotButton = new wxButton(panel, ID_OpenChatbot, "Open Chatbot"); // New button
    vbox->Add(chatbotButton, 0, wxALL | wxCENTER, 10);

    output = new wxTextCtrl(panel, wxID_ANY, "", wxDefaultPosition, wxSize(480, 250), wxTE_MULTILINE | wxTE_READONLY);
    vbox->Add(output, 1, wxALL | wxEXPAND, 10);

    panel->SetSizer(vbox);
}

size_t WriteCallback(void* contents, size_t size, size_t nmemb, void* userp) {
    ((std::string*)userp)->append((char*)contents, size * nmemb);
    return size * nmemb;
}

void MyFrame::OnOpenChatbot(wxCommandEvent& event) {
    wxDialog* chatbotDialog = new wxDialog(this, wxID_ANY, "Chatbot", wxDefaultPosition, wxSize(400, 300));
    wxBoxSizer* vbox = new wxBoxSizer(wxVERTICAL);

    wxTextCtrl* chatbotInput = new wxTextCtrl(chatbotDialog, wxID_ANY, "", wxDefaultPosition, wxSize(380, 30));
    vbox->Add(chatbotInput, 0, wxALL | wxEXPAND, 10);

    wxTextCtrl* chatbotOutput = new wxTextCtrl(chatbotDialog, wxID_ANY, "", wxDefaultPosition, wxSize(380, 200), wxTE_MULTILINE | wxTE_READONLY);
    vbox->Add(chatbotOutput, 1, wxALL | wxEXPAND, 10);

    wxButton* sendButton = new wxButton(chatbotDialog, wxID_ANY, "Send");
    vbox->Add(sendButton, 0, wxALL | wxCENTER, 10);

    chatbotDialog->SetSizer(vbox);

    sendButton->Bind(wxEVT_BUTTON, [chatbotInput, chatbotOutput](wxCommandEvent&) {
        wxString userMessage = chatbotInput->GetValue();
        std::string response = "I don't understand that command.";

        // Convert wxString to std::string
        std::string userMessageStr = std::string(userMessage.mb_str());
        std::cerr << "User message: " << userMessageStr << std::endl;  // Debug output

        // Prepare JSON payload
        std::string jsonPayload = "{\"message\": \"" + userMessageStr + "\"}";

        // Send the user message to the Rasa server
        CURL* curl;
        CURLcode res;
        std::string readBuffer;

        curl = curl_easy_init();
        if(curl) {
            curl_easy_setopt(curl, CURLOPT_URL, "http://localhost:5005/webhooks/rest/webhook");
            curl_easy_setopt(curl, CURLOPT_POSTFIELDS, jsonPayload.c_str());
            curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
            curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);
            struct curl_slist *headers = NULL;
            headers = curl_slist_append(headers, "Content-Type: application/json");
            curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

            res = curl_easy_perform(curl);
            if(res == CURLE_OK) {
                std::cerr << "Response received: " << readBuffer << std::endl; // Debug output
                response = readBuffer;
            } else {
                std::cerr << "curl_easy_perform() failed: " << curl_easy_strerror(res) << std::endl;
            }
            curl_easy_cleanup(curl);
        } else {
            std::cerr << "curl_easy_init() failed" << std::endl;
        }

        std::cerr << "Final response: " << response << std::endl; // Debug output
        chatbotOutput->AppendText("You: " + userMessage + "\n");
        chatbotOutput->AppendText("Bot: " + wxString(response) + "\n");
        chatbotInput->Clear();
    });

    chatbotDialog->ShowModal();
    chatbotDialog->Destroy();
}

void MyFrame::OnExecute(wxCommandEvent& event) {
    wxString cmd = input->GetValue();
    std::string result;

    SimpleShell shell;
    shell.executeCommand(std::string(cmd.mb_str()), result, 1);

    // Debug: Print the result to the console
    std::cout << "Command: " << std::string(cmd.mb_str()) << std::endl;
    std::cout << "Result: " << result << std::endl;

    // Debug: Check if result is being set
    if (result.empty()) {
        std::cout << "Result is empty" << std::endl;
    } else {
        std::cout << "Result is not empty" << std::endl;
    }

    // Set the result to the output text control
    output->SetValue(result);
}