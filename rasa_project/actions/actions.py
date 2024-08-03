from typing import Any, Text, Dict, List
from rasa_sdk import Action, Tracker
from rasa_sdk.executor import CollectingDispatcher

COMMAND_RESPONSES = {
    "concatenate": "The syntax for 'concatenate' is: concatenate <file1> [file2]",
    "word_count": "The syntax for 'word_count' is: word_count <file>",
    "list_everything": "The syntax for 'list_everything' is: list_everything [directory]",
    "print directory": "The syntax for 'print directory' is: print directory",
    "make_directory": "The syntax for 'make_directory' is: make_directory <directory>",
    "change directory": "The syntax for 'change directory' is: change directory <directory>",
    "clear": "The syntax for 'clear' is: clear",
    "move_file": "The syntax for 'move_file' is: move_file <source> <destination>",
    "create_file": "The syntax for 'create_file' is: create_file <file>",
    "copy": "The syntax for 'copy' is: copy <source> <destination>",
    "match_pattern": "The syntax for 'match_pattern' is: match_pattern <pattern> <file>",
    "change_mode": "The syntax for 'change_mode' is: change_mode <permissions> <file>",
    "remove": "The syntax for 'remove' is: remove <file>",
    "print_head": "The syntax for 'print_head' is: print_head <file>",
    "print_tail": "The syntax for 'print_tail' is: print_tail <file>",
    "difference": "The syntax for 'difference' is: difference <file1> <file2>",
    "reverse": "The syntax for 'reverse' is: reverse <file>",
    "uppercase": "The syntax for 'uppercase' is: uppercase <file>",
    "lowercase": "The syntax for 'lowercase' is: lowercase <file>",
    "findreplace": "The syntax for 'findreplace' is: findreplace <file> <search> <replace>",
    "encrypt": "The syntax for 'encrypt' is: encrypt <file> <shift>",
    "decrypt": "The syntax for 'decrypt' is: decrypt <file> <shift>",
    "compress": "The syntax for 'compress' is: compress <file>",
    "decompress": "The syntax for 'decompress' is: decompress <file>",
    "filehistory": "The syntax for 'filehistory' is: filehistory <file>",
    "filesplit": "The syntax for 'filesplit' is: filesplit <file> <size/lines>",
    "filejoin": "The syntax for 'filejoin' is: filejoin <file1> <file2> ... <output_file>"
}

class ActionProvideCommand(Action):
    def name(self) -> Text:
        return "action_provide_command"

    def run(self, dispatcher: CollectingDispatcher,
            tracker: Tracker,
            domain: Dict[Text, Any]) -> List[Dict[Text, Any]]:
        command = next(tracker.get_latest_entity_values("command"), None)
        if command in COMMAND_RESPONSES:
            response = COMMAND_RESPONSES[command]
            dispatcher.utter_message(text=response)
        else:
            dispatcher.utter_message(text="Sorry, I don't know that command.")
        return []