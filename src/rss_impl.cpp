#include "rss_impl.h"
#include "iostream"

using namespace std;

RSSImpl::RSSImpl(Collection& coll, const vector<string>& keys)
    : coll_(coll), keys_(keys) {
  this->active_ = 0;
  this->counter_ = 0;
  this->bucket_size_ = 0;
}

void RSSImpl::init() {
  for (size_t i = 0; i < HEAP_SIZE && this->counter_ < this->coll_.size();
       i += 1) {
    cout << this->stats() << endl;
    Record rec = this->coll_.get(this->counter_);
    this->heap_.push_back(rec);
    this->counter_ += 1;
    this->active_ += 1;
  }
}

bool RSSImpl::finished() {
  return this->counter_ >= this->coll_.size() && this->heap_.empty();
}

Record RSSImpl::pop() {
  Record record = this->heap_[0];
  this->heap_[0] = this->heap_[this->active_ - 1];
  this->bucket_size_ += 1;

  this->update(record);

  return record;
}

void RSSImpl::update(Record& last) {
  if (this->counter_ < this->coll_.size()) {
    Record next = this->fetch(this->counter_);
    this->heap_[this->active_ - 1] = next;
    this->counter_ += 1;

    if (next.compare(last, this->keys_) < 0) {
      this->active_ -= 1;
    }
  } else {
    vector<Record>::iterator it = this->heap_.begin();
    it += this->active_ - 1;

    cout << "Deleting: " << this->stringify_heap() << " " << this->heap_.size() << endl;
    this->heap_.erase(it);
    cout << "Deleted: " << this->stringify_heap() << " " << this->heap_.size() << endl;
  }
  if (this->active_ == 0 || this->heap_.empty()) {
    this->bucket_counts_.push_back(this->bucket_size_);
    this->bucket_size_ = 0;
    this->active_ = this->heap_.size();
  }
}

string RSSImpl::stats() {
  ostringstream ss;
  ss << "Counter=" << this->counter_;
  ss << " ";
  ss << "Active=" << this->active_;
  ss << " ";
  ss << "Heap size=" << this->heap_.size();
  ss << " ";
  if (this->counter_ < this->coll_.size()) {
    string next = this->coll_.get(this->counter_).str();
    ss << "Peek=" << next[17] << next[18] << "...";
  }
  return ss.str();
}

void RSSImpl::heapify() {
  if (this->active_ <= 1) {
    return;
  }

  bool done = false;
  while (!done) {
    done = true;
    for (size_t i = this->active_; (i / 2) != 0;) {
      size_t p = (i / 2) - 1;
      size_t l = (i - 1) - 1;
      size_t r = (i)-1;

      if (i % 2 == 0) {
        Record parent = this->heap_[p];
        Record child = this->heap_[i - 1];
        if (child.compare(parent, this->keys_) < 0) {
          this->heap_[p] = child;
          this->heap_[i - 1] = parent;
          done = false;
        }
        i -= 1;
      } else {
        Record parent = this->heap_[p];
        Record left = this->heap_[l];
        Record right = this->heap_[r];
        if (left.compare(parent, this->keys_) < 0) {
          this->heap_[p] = left;
          this->heap_[l] = parent;
          done = false;
        } else if (right.compare(parent, this->keys_) < 0) {
          this->heap_[p] = right;
          this->heap_[r] = parent;
          done = false;
        }
        i -= 2;
      }
    }
  }
}

vector<size_t> RSSImpl::bucket_counts() { return this->bucket_counts_; }

string RSSImpl::stringify_heap() {
  ostringstream ss;
  if (this->heap_.size() == 0) {
    return "[]";
  }

  size_t heap_size = this->heap_.size();

  ss << "[";
  for (size_t i = 0; i < heap_size; i += 1) {
    ss << this->heap_[i].row();
    if (heap_size != this->active_ && i == this->active_ - 1) {
      ss << " | ";
    } else if (i < heap_size - 1) {
      ss << ", ";
    }
  }
  // for (size_t i = 0; i < this->active_; i += 1) {
  //   ss << this->heap_[i].row();
  //   if (i < this->active_ - 1) {
  //     ss << ", ";
  //   }
  // }
  // if (this->active_ != this->heap_.size()) {
  //   ss << " | ";
  // }
  // for (size_t i = this->active_; i < this->heap_.size(); i += 1) {
  //   ss << this->heap_[i].row();
  //   if (i < this->heap_.size() - 1) {
  //     ss << ", ";
  //   }
  // }
  ss << "]";
  return ss.str();
}

Record RSSImpl::fetch(size_t i) { return this->coll_.get(i); }
