from flask import Flask
from flask import jsonify
from flask import render_template
from flask import request
import json



app = Flask(__name__)

@app.route('/')
def hello_world():
        return 'Hello , World !\n'

welcome = "Welcome to 3ESE API!"

@app.route('/api/welcome/')
def api_welcome():
        return welcome

@app.route('/api/welcome/<int:index>')
def api_welcome_index(index):

#       return welcome[index]
#       return json.dumps({"index": index, "val": welcome[index]}),{"Content-Type": "application/json"}
        if index>19:
                return page_not_found(404)
        else :
                return jsonify({"index": index, "val": welcome[index]})

@app.errorhandler(404)
def page_not_found(error):
        return render_template('page_not_found.html'),404

@app.route('/api/welcome/<int:index>', methods=['GET','POST'])
@app.route('/api/welcome', methods=['GET','POST'])

@app.route('/api/request/', methods=['GET', 'POST'])
@app.route('/api/request/<path>', methods=['GET','POST'])
def api_request(path=None):
    resp = {
            "method":   request.method,
            "url" :  request.url,
            "path" : path,
            "args": request.args,
            "headers": dict(request.headers),
    }
    if request.method == 'POST':
        resp["POST"] = {
                "data" : request.get_json(),
                "args" : request.get_json(),
                }
    return jsonify(resp)

@app.route('/api/welcome/<int:index>',methods=['PUT'])
def api_put(index):
        global welcome
        data = request.get_json()
        if index > len(welcome):
                abort(404)
        else:
                #construction de la nouvelle chaine
                welcome = welcome[:index] + data + welcome[index:]
                return welcome + '\r\n'
