from ucomgen.ucom_definitions import UcomTestMessage,UcomTestSignal,OutputTypes
import json

class DBULoader:
    """Loads messages from a DBU file"""
    def __init__(self, dbu_path: str) -> None:
        self.__dbu_path = dbu_path
        self.__dbu_data = None
        self.__messages = None
        
    def load(self) -> bool:
        """Loads the DBU file specified in the constructor and stores is as 
        a JSON object"""
        try:
            with open(self.__dbu_path) as f:
                self.__dbu_data = json.loads(f.read())
            return True
        except Exception as e:
            message = e.message if hasattr(e, 'message') else e
            print(f"Failed to load DBU file : {message}")
            return False

    def get_messages(self) -> dict[int,UcomTestMessage]:
        """Returns a dictionary (with ID as key) of the messages in the DBU"""
        if self.__dbu_data is None:
            return {}
        
        if self.__messages is not None:
            return self.__messages
    
        messages_in_dbu = None
        messages = {}
        try:
            if 'Messages' in self.__dbu_data:
                messages_in_dbu = self.__dbu_data['Messages']
        except Exception as e:
            print(e)
            return {}

        try:
            if messages_in_dbu is not None:
                for message in messages_in_dbu:
                    signals = []
                    message_id = message['MessageID']
                    message_version = message['MessageVersion']
                    output_type : OutputTypes = OutputTypes[message['OutputType']]
                    for s in message['SignalsInMessage']:
                        signals.append(UcomTestSignal(s['SignalID'], s['DataType']))
                    ucom_message = UcomTestMessage(message_id, message_version, message['MessageEnabled'], output_type, signals, message['MessageTiming'])                   
                    messages[ucom_message.uid()] = ucom_message
        except Exception as e:
            print(e)
            return {}

        if len(messages) > 0:
            self.__messages = messages
            return messages
        
        return {} 
    
    def get_signals_in_message(self, message_id : str) -> list[UcomTestSignal]:
        """Gets a list of the signals in a message"""
        if self.__messages is None or self.__dbu_data is None:
            return []
        
        try:
            
            if messages_in_dbu is not None:
                for message in messages_in_dbu:
                    if 'SignalsInMessage' in message:
                        signals_in_message = message['SignalsInMessage']
                        for s in signals_in_message:
                            messages[s['SignalID']] = UcomTestMessage()
        except Exception as e:
            print(e)
            return {}
        return {}
     