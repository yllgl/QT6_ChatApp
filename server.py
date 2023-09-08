from flask import Flask, request, jsonify
from flask_sqlalchemy import SQLAlchemy
from datetime import datetime, timedelta
from flask_apscheduler import APScheduler
from sqlalchemy.orm import joinedload
from werkzeug.security import generate_password_hash, check_password_hash
app = Flask(__name__)
app.config['SQLALCHEMY_DATABASE_URI'] = 'sqlite:///chat.db'  # 使用SQLite数据库
db = SQLAlchemy(app)
import socket
import json
UDP_PORT = 12345
class User(db.Model):
    id = db.Column(db.Integer, primary_key=True)
    username = db.Column(db.String(80), unique=True, nullable=False)
    password_hash = db.Column(db.String(256), nullable=False)  # replaced password with password_hash
    nickname = db.Column(db.String(80), nullable=True)
    online = db.Column(db.Boolean, default=False)
    lastip = db.Column(db.String(45), nullable=True)  # adjusted size for IPv6 compatibility
    last_heartbeat = db.Column(db.DateTime, default=datetime.now)
    isdeleted = db.Column(db.Boolean, default=False)
    @property
    def password(self):
        raise AttributeError('password is not a readable attribute')

    @password.setter
    def password(self, password):
        self.password_hash = generate_password_hash(password)

    def verify_password(self, password):
        return check_password_hash(self.password_hash, password)
class ChatGroup(db.Model):
    id = db.Column(db.Integer, primary_key=True)
    name = db.Column(db.String(80), nullable=False)

class GroupMembers(db.Model):
    id = db.Column(db.Integer, primary_key=True)
    user_id = db.Column(db.Integer, db.ForeignKey('user.id'), nullable=False)
    group_id = db.Column(db.Integer, db.ForeignKey('chat_group.id'), nullable=False)


class Friendship(db.Model):
    id = db.Column(db.Integer, primary_key=True)
    user1_id = db.Column(db.Integer, db.ForeignKey('user.id'), nullable=False)
    user2_id = db.Column(db.Integer, db.ForeignKey('user.id'), nullable=False)
    __table_args__ = (db.UniqueConstraint('user1_id', 'user2_id', name='unique_friendship'),)

    

class Message(db.Model):
    id = db.Column(db.Integer, primary_key=True)
    content = db.Column(db.Text, nullable=False)
    timestamp = db.Column(db.DateTime, default=datetime.now)
    author_id = db.Column(db.Integer, db.ForeignKey('user.id'), nullable=False)
    group_id = db.Column(db.Integer, db.ForeignKey('chat_group.id'), nullable=True)
    friendship_id = db.Column(db.Integer, db.ForeignKey('friendship.id'), nullable=True)

    # Add constraints to ensure that a message is either from a group or a friendship, not both.
    __table_args__ = (
        db.CheckConstraint(
            '(group_id IS NULL AND friendship_id IS NOT NULL) OR (group_id IS NOT NULL AND friendship_id IS NULL)',
            name='message_source_check'
        ),
    )


@app.route('/register', methods=['POST'])
def register():
    username = request.json.get('username')
    password = request.json.get('password')
    nickname = request.json.get('nickname')
    if User.query.filter_by(username=username).first():
        return jsonify({"message": "Username already exists!"}), 400
    if username == "":
        return jsonify({"message": "Username cannot be empty!"}), 400
    user = User(username=username, password=password, nickname=nickname)
    db.session.add(user)
    db.session.commit()
    return jsonify({"message": "User registered successfully!"})

@app.route('/login', methods=['POST'])
def login():
    username = request.json.get('username')
    password = request.json.get('password')
    
    user = User.query.filter_by(username=username).first()
    if user and user.verify_password(password):
        ip = request.headers.get('X-Forwarded-For', request.remote_addr)
        user.online = True
        user.lastip = ip
        user.last_heartbeat = datetime.now()
        db.session.commit()
        friends = [{"nickname":User.query.get(friendship.user2_id).nickname,"user_id":friendship.user2_id,"username":User.query.get(friendship.user2_id).username} for friendship in Friendship.query.filter_by(user1_id=user.id).all()]+\
            [{"nickname":User.query.get(friendship.user1_id).nickname,"user_id":friendship.user1_id,"username":User.query.get(friendship.user1_id).username} for friendship in Friendship.query.filter_by(user2_id=user.id).all()]
        groups = GroupMembers.query.filter_by(user_id=user.id).all()
        group_info = {group.group_id: {"name":ChatGroup.query.get(group.group_id).name,"member":[{"user_id":member.user_id,"nickname":User.query.get(member.user_id).nickname,"username":User.query.get(member.user_id).username} 
                                                                                                 for member in GroupMembers.query.filter_by(group_id=group.group_id).all()]} for group in groups}
        return jsonify({
            "friends": friends,
            "group_info": group_info,
            "message": "Logged in successfully!", "user_id": user.id, "nickname": user.nickname, "username": user.username
        })
    return jsonify({"message": "Invalid credentials!"}), 401

@app.route('/user_info', methods=['POST'])
def get_user_info():
    username = request.json.get('username', None)
    user_id = request.json.get('user_id', None)
    assert username or user_id, "Either username or user_id must be provided!"
    if username:
        user = User.query.filter_by(username=username).first()
    else:
        user = User.query.get(user_id)
    friends = [{"nickname":User.query.get(friendship.user2_id),"user_id":friendship.user2_id} for friendship in Friendship.query.filter_by(user1_id=user.id).all()]+\
            [{"nickname":User.query.get(friendship.user1_id),"user_id":friendship.user1_id} for friendship in Friendship.query.filter_by(user2_id=user.id).all()]
    groups = GroupMembers.query.filter_by(user_id=user.id).all()
    group_info = {group.group_id: {"name":ChatGroup.query.get(group.group_id).name,"member":[{"user_id":member.user_id,"nickname":User.query.get(member.user_id).nickname} for member in GroupMembers.query.filter_by(group_id=group.group_id).all()]} for group in groups}
    return jsonify({
        "friends": friends,
        "group_info": group_info,
        "message": "Logged in successfully!", "user_id": user.id, "nickname": user.nickname, "username": user.username
    })

@app.route('/add_friend', methods=['POST'])
def add_friend():
    user_id = request.json.get('user_id', None)
    friend_id = request.json.get('friend_id', None)
    friend_username = request.json.get('friend_username', None)
    assert friend_id or friend_username, "Either friend_id or friend_username must be provided!"
    if friend_username:
        friend = User.query.filter_by(username=friend_username).first()
    else:
        friend = User.query.get(friend_id)
    if not friend:
        return jsonify({"message": "Friend does not exist!"}), 404
    friend_id = friend.id
    if Friendship.query.filter_by(user1_id=user_id, user2_id=friend_id).first() or Friendship.query.filter_by(user1_id=friend_id, user2_id=user_id).first():
        return jsonify({"message": "Friend already exists!"}), 400
    if user_id == friend_id:
        return jsonify({"message": "Cannot add yourself as a friend!"}), 400
    friendship = Friendship(user1_id=user_id, user2_id=friend_id)
    db.session.add(friendship)
    db.session.commit()
    friend_user = User.query.get(friend_id)
    if friend_user.online:
        serverSocket = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
        send_message = json.dumps({"other_user_add_friend":True,"message": "You have a new friend request from {}!".format(User.query.get(user_id).nickname),"user_id":user_id,"username":User.query.get(user_id).username,"nickname":User.query.get(user_id).nickname})
        serverSocket.sendto(send_message.encode(), (friend_user.lastip, UDP_PORT))
    return jsonify({"message": "Friend added successfully!", "user_id": friend_id, "username": friend_user.username, "nickname": friend_user.nickname})
def send_join_group(author_id,group_id):
    membership = GroupMembers.query.filter_by(user_id=author_id, group_id=group_id).first()
    if not membership:
        return False
    serverSocket = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    users = (
        db.session.query(User).filter(User.id != author_id)  # Exclude the author
        .join(GroupMembers, GroupMembers.user_id == User.id)
        .filter(GroupMembers.group_id == group_id)
        .filter(User.online == True)
        .all()
    )

    # Extract the lastip addresses from the result
    ips = [user.lastip for user in users]
    send_message = json.dumps({"other_user_join_group":True,"author_id":author_id,"username":User.query.get(author_id).username,"nickname":User.query.get(author_id).nickname,"group_id":group_id})
    for ip in ips:
        serverSocket.sendto(send_message.encode(), (ip, UDP_PORT))
    return True

@app.route('/join_group', methods=['POST'])
def join_group():
    user_id = request.json.get('user_id', None)
    group_id = request.json.get('group_id', None)
    assert user_id and group_id, "Both user_id and group_id must be provided!"
    if not ChatGroup.query.get(group_id):
        group = ChatGroup(id=group_id, name="Group {}".format(group_id))
        db.session.add(group)
    group_member = GroupMembers(user_id=user_id, group_id=group_id)
    db.session.add(group_member)
    db.session.commit()
    if not send_join_group(user_id,group_id):
        return jsonify({"message": "User is not a member of the group!"}), 404
    return jsonify({"message": "Joined group successfully!", "group_id": group_id, "group_name": ChatGroup.query.get(group_id).name,
                    "group_member": [{"user_id":member.user_id,"nickname":User.query.get(member.user_id).nickname,"username":User.query.get(member.user_id).username} for member in GroupMembers.query.filter_by(group_id=group_id).all()],
                    "group_message": [{"content":message.content,"author_id":message.author_id,"timestamp":message.timestamp.isoformat(),"nickname":User.query.get(message.author_id).nickname,"username":User.query.get(message.author_id).username} 
                                      for message in Message.query.filter_by(group_id=group_id).order_by(Message.timestamp.asc()).all()]})
@app.route('/list_group', methods=['GET'])
def list_group():
    return jsonify({"group_list": [{"group_id":group.id,"group_name":group.name} for group in ChatGroup.query.all()]})
@app.route('/list_group_members/<int:group_id>', methods=['GET'])
def list_group_members(group_id):
    return jsonify({"group_member": [{"user_id":member.user_id,"nickname":User.query.get(member.user_id).nickname,"username":User.query.get(member.user_id).username} for member in GroupMembers.query.filter_by(group_id=group_id).all()]})
@app.route('/list_user', methods=['GET'])
def list_user():
    return jsonify({"user_list": [{"user_id":user.id,"nickname":user.nickname,"username":user.username} for user in User.query.all()]})
@app.route('/list_message', methods=['GET'])
def list_message():
    return jsonify({"group_message_list": [{"content":message.content,"author_id":message.author_id,"timestamp":message.timestamp.isoformat(),"nickname":User.query.get(message.author_id).nickname,"username":User.query.get(message.author_id).username,
                                      "group_id":message.group_id} for message in Message.query.filter_by(friendship_id=None).order_by(Message.timestamp.asc()).all()],
                    "friend_message_list":[{"content":message.content,"author_id":message.author_id,"timestamp":message.timestamp.isoformat(),"author_nickname":User.query.get(message.author_id).nickname,"author_username":User.query.get(message.author_id).username,
                                            "friend_id":(Friendship.query.get(message.friendship_id).user1_id if Friendship.query.get(message.friendship_id).user1_id!=message.author_id else Friendship.query.get(message.friendship_id).user2_id),
                                            "friend_username":User.query.get((Friendship.query.get(message.friendship_id).user1_id if Friendship.query.get(message.friendship_id).user1_id!=message.author_id else Friendship.query.get(message.friendship_id).user2_id)).username,
                                            "friend_nickname":User.query.get((Friendship.query.get(message.friendship_id).user1_id if Friendship.query.get(message.friendship_id).user1_id!=message.author_id else Friendship.query.get(message.friendship_id).user2_id)).nickname} for message in Message.query.filter_by(group_id=None).order_by(Message.timestamp.asc()).all()]})
@app.route('/get_friend_message', methods=['POST'])
def get_friend_message():
    user_id = request.json.get('user_id', None)
    friend_id = request.json.get('friend_id', None)
    assert user_id and friend_id, "Both user_id and friend_id must be provided!"
    friendship = Friendship.query.filter(
        ((Friendship.user1_id == user_id) & (Friendship.user2_id == friend_id)) |
        ((Friendship.user1_id == friend_id) & (Friendship.user2_id == user_id))
    ).first()
    if not friendship:
        return jsonify({"message": "Friend does not exist!"}), 404
    friendship_id = friendship.id
    return jsonify({"friend_message_list":[{"content":message.content,"author_id":message.author_id,"timestamp":message.timestamp.isoformat(),"nickname":User.query.get(message.author_id).nickname,"username":User.query.get(message.author_id).username} for message in Message.query.filter_by(friendship_id=friendship_id).order_by(Message.timestamp.asc()).all()],
                    "friend_id":friend_id,"friend_username":User.query.get(friend_id).username,"friend_nickname":User.query.get(friend_id).nickname})
@app.route('/get_group_message', methods=['POST'])
def get_group_message():
    user_id = request.json.get('user_id', None)
    group_id = request.json.get('group_id', None)
    assert user_id and group_id, "Both user_id and group_id must be provided!"
    group_member = GroupMembers.query.filter_by(user_id=user_id, group_id=group_id).first()
    if not group_member:
        return jsonify({"message": "User is not a member of the group!"}), 404
    return jsonify({"group_message_list":[{"content":message.content,"author_id":message.author_id,"timestamp":message.timestamp.isoformat(),"nickname":User.query.get(message.author_id).nickname,"username":User.query.get(message.author_id).username} for message in Message.query.filter_by(group_id=group_id).order_by(Message.timestamp.asc()).all()],
                    "group_member":[{"user_id":member.user_id,"nickname":User.query.get(member.user_id).nickname,"username":User.query.get(member.user_id).username} for member in GroupMembers.query.filter_by(group_id=group_id).all()],
                    "group_name":ChatGroup.query.get(group_id).name,"group_id":group_id})


@app.route('/remove_friend', methods=['POST'])
def remove_friend():
    user_id = request.json.get('user_id', None)
    friend_id = request.json.get('friend_id', None)
    assert user_id and friend_id, "Both user_id and friend_id must be provided!"
    friendship = Friendship.query.filter(
        ((Friendship.user1_id == user_id) & (Friendship.user2_id == friend_id)) |
        ((Friendship.user1_id == friend_id) & (Friendship.user2_id == user_id))
    ).first()
    if not friendship:
        return jsonify({"message": "Friend does not exist!"}), 404
    db.session.delete(friendship)
    db.session.commit()
    return jsonify({"message": "Friend removed successfully!"})

@app.route('/leave_group', methods=['POST'])
def leave_group():
    user_id = request.json.get('user_id', None)
    group_id = request.json.get('group_id', None)
    assert user_id and group_id, "Both user_id and group_id must be provided!"
    group_member = GroupMembers.query.filter_by(user_id=user_id, group_id=group_id).first()
    if not group_member:
        return jsonify({"message": "User is not a member of the group!"}), 404
    db.session.delete(group_member)
    db.session.commit()
    return jsonify({"message": "Left group successfully!"})

@app.route('/delete_account', methods=['POST'])
def delete_account():
    user_id = request.json.get('user_id', None)
    user = User.query.get(user_id)
    user.isdeleted = True
    db.session.commit()
    return jsonify({"message": "Account deleted successfully!"})
@app.route('/send_private_message', methods=['POST'])
def send_private_message():
    content = request.json.get('content')
    sender_id = request.json.get('user_id')
    receiver_id = request.json.get('friend_id')
    
    # 验证好友关系
    friendship = Friendship.query.filter(
        ((Friendship.user1_id == sender_id) & (Friendship.user2_id == receiver_id)) | 
        ((Friendship.user1_id == receiver_id) & (Friendship.user2_id == sender_id))
    ).first()
    if not friendship:
        return jsonify({"message": "The users are not friends!"}), 404
    
    message = Message(content=content, author_id=sender_id, friendship_id=friendship.id)
    db.session.add(message)
    if User.query.get(receiver_id).online:
        serverSocket = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
        send_message = json.dumps({"content":content,"author_id":sender_id,"timestamp":message.timestamp.isoformat(),"nickname":User.query.get(sender_id).nickname,
                                   "username":User.query.get(sender_id).username,"from_friend_id":sender_id})
        serverSocket.sendto(send_message.encode(), (User.query.get(receiver_id).lastip, UDP_PORT))
    db.session.commit()
    
    return jsonify({"message": "Private message sent successfully!"})

@app.route('/send_group_message', methods=['POST'])
def send_group_message():
    content = request.json.get('content')
    author_id = request.json.get('user_id')
    group_id = request.json.get('group_id')
    
    # 验证群组成员关系
    membership = GroupMembers.query.filter_by(user_id=author_id, group_id=group_id).first()
    if not membership:
        return jsonify({"message": "User is not a member of the group!"}), 404
    
    message = Message(content=content, author_id=author_id, group_id=group_id)
    db.session.add(message)
    
    
    serverSocket = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    users = (
        db.session.query(User).filter(User.id != author_id)  # Exclude the author
        .join(GroupMembers, GroupMembers.user_id == User.id)
        .filter(GroupMembers.group_id == group_id)
        .filter(User.online == True)
        .all()
    )

    # Extract the lastip addresses from the result
    ips = [user.lastip for user in users]
    send_message = json.dumps({"content":content,"author_id":author_id,"nickname":User.query.get(author_id).username,"timestamp":message.timestamp.isoformat(),"nickname":User.query.get(author_id).nickname,"group_id":group_id})
    for ip in ips:
        serverSocket.sendto(send_message.encode(), (ip, UDP_PORT))
    db.session.commit()
    
    return jsonify({"message": "Group message sent successfully!"})

@app.route('/heartbeat', methods=['POST'])
def heartbeat():
    data = request.get_json()
    username = data.get('username')

    user = User.query.filter_by(username=username).first()
    if not user:
        return jsonify({"message": "User does not exist!"}), 400

    user.last_heartbeat = datetime.now()
    user.online = True
    user.lastip = request.headers.get('X-Forwarded-For', request.remote_addr)
    db.session.commit()

    return jsonify({"message": "Heartbeat received!"})
def check_online_status():
    with app.app_context():
        timeout = timedelta(minutes=5)  # Assuming 5 minutes as the timeout period
        now = datetime.now()

        for user in User.query.filter_by(online=True).all():
            if now - user.last_heartbeat > timeout:
                user.online = False
            else:
                user.online = True

        db.session.commit()
with app.app_context():
    scheduler = APScheduler()
    db.create_all()
    scheduler.add_job(id='check_online_status_job', func=check_online_status, trigger='interval', minutes=5)
    scheduler.start()
if __name__ == '__main__':
    
    app.run(host='0.0.0.0',debug=True)
