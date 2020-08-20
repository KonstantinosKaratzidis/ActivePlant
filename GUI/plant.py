class Plant:
    def __init__(self, name, address):
        self.name = name
        self.address = address

def import_plants():
    """Returns a list of plants"""
    return [Plant("Test Plant", 0xa0)]
