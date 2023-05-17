import tkinter as tk
 
# GUIの生成
root = tk.Tk()
 
# タイトル
root.title(u"app with python")
 
# サイズ
root.geometry("400x300")
 
# windowを閉じる
def close_window():
    root.destroy()
 
# ボタン
btn = tk.Button(root, text=u'button', command=close_window)
# ボタンの位置指定
btn.place(x=150, y=180)
 
root.mainloop()
